#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "pixel_format.hpp"
#include "matrix.hpp"


using pixel_storage_uchar3 = basic_matrix<uint8_t, 3>;

#include <chrono> // для функций из std::chrono

class Timer
{
private:
	// Псевдонимы типов используются для удобного доступа к вложенным типам
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;
	
	std::chrono::time_point<clock_t> m_beg;
 
public:
	Timer() : m_beg(clock_t::now())
	{
	}
	
	void reset()
	{
		m_beg = clock_t::now();
	}
	
	double elapsed() const
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};

template <class T>
T clamp(const T min, const T max, const T val)
{
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

template <class T>
T max(const T a, const T b)
{
    return a > b ? a : b;
}


typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned int    dword;

constexpr word bitmap_signature = 0x4d42;

#pragma pack(push, 1)
struct bitmap_file_header   /* 14 bytes */
{
    /* The file signature; must be BM */
    word        signature{bitmap_signature};
    /* The size, in bytes, of the bitmap file */
    dword       size{0}; 
    /* Reserved; must be zero */
    word        reserved1{0};
    word        reserved2{0};
    /* The offset, in bytes, from the beginning of the bitmap_file_header structure to the bitmap bits */
    dword       offset{0};
};

struct bitmap_info_header /* 40 bytes */
{
    /* The number of bytes required by the structure */
    dword       size{sizeof(bitmap_info_header)};
    /* The width and the height of the bitmap, in pixels. If the height is positive, the bitmap is a bottom-up DIB and its
    * origin is the lower-left corner. If the height is negative, the bitmap is a top-down DIB and its origin is the
    * upper-left corner*/
    int         width{0};
    int         height{0};    
    /* The number of planes for the target device. This value must be set to 1 */
    word        planes{1};
    /* The number of bits-per-pixel. The bit_count member of the BITMAPINFOHEADER structure determines the number of bits that
    * define each pixel and the maximum number of colors in the bitmap. This member must be one of the following values */
    word        bitCount{0};   
    /* Compression: 0 - BI_RGB, 1 - BI_RLE8, 2 - BI_RLE4 */ 
    dword       compression{0};
    /* The size, in bytes, of the image. This may be set to zero for BI_RGB bitmaps */
    dword       imageSize{0}; 
    /* The horizontal and the vertical resolution, in pixels-per-meter, of the target device for the bitmap. An application
    * can use this value to select a bitmap from a resource group that best matches the characteristics of the current device */
    int         xPxPerMeter{0};
    int         yPxPerMeter{0};
    /* The number of color indexes in the color table that are actually used by the bitmap. If this value is zero, the bitmap
    * uses the maximum number of colors corresponding to the value of the biBitCount member for the compression mode specified
    *  by biCompression */
    dword       colorUsed{0};
    /* The number of color indexes that are required for displaying the bitmap. If this value is zero, all colors are required */
    dword       colorImportant{0};
};

struct bitmap_header
{
    bitmap_file_header  header;
    bitmap_info_header  info;
};
#pragma pack(pop)

bool bitmap_save_to_file(const pixel_storage_uchar3& image, const std::string& filename)
{
    bitmap_header full;

    /* Set bitmap file header */
    full.header.offset = static_cast<dword>(sizeof(full));
    full.header.size = full.header.offset + image.get_bytes_per_row() * image.get_rows();
    /* Set bitmap info header */
    full.info.width = image.get_columns();
    full.info.height = image.get_rows();
    full.info.bitCount = static_cast<word>(image.get_bytes_per_cell() * 8);
    full.info.imageSize = image.get_bytes_per_row() * image.get_rows();

    std::fstream file;
    file.open(filename, std::ios_base::out | std::ios_base::binary);

    /* Write file and info headers */
    if (!file.write(static_cast<char*>(static_cast<void*>(&full)), sizeof(full))) {
        std::cout << "image::save_to_file() error: writing error (full)" << std::endl;
        return false;
    }

    /* Write bitmap data */
    for (int i = 0; i < image.get_rows(); i++) {
        char* data = static_cast<char*>(static_cast<void*>(image.get_row_ptr(image.get_rows() - i - 1)));
        if (!file.write(data, image.get_bytes_per_row())) {
            std::cout << "image::save_to_file() error: writing error (info)" << std::endl;
            return false;
        }
    }
    return true;
}



class vec3
{
public:
    vec3();
    vec3(float x, float y, float z);

    vec3& operator*=(float val);

    vec3 cross_product(const vec3& val) const;
    float dot_product(const vec3& val) const;
    float length();
    
    float normalize();
    vec3& normalize_self();

public:
    float x, y, z;
};

vec3::vec3()
    : x(0.0)
    , y(0.0)
    , z(0.0)
{
}

vec3::vec3(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
{
}

vec3& vec3::operator*=(float val)
{
    x *= val;
    y *= val;
    z *= val;
    return *this;
}

// ^
vec3 vec3::cross_product(const vec3& val) const
{
    return vec3(y * val.z - z * val.y, z * val.x - x * val.z, x * val.y - y * val.x);
}

// %
float vec3::dot_product(const vec3& val) const
{
    return x * val.x + y * val.y + z * val.z;
}

float vec3::normalize()
{
    auto sq = x * x + y * y + z * z;
    auto inv = std::sqrt(1.0 / sq);
    *this *= inv;
    return inv * sq;
}

vec3& vec3::normalize_self()
{
    auto sq = x * x + y * y + z * z;
    auto inv = std::sqrt(1.0 / sq);
    *this *= inv;
    return *this;
}

vec3 operator-(const vec3& val)
{
    return vec3(-val.x, -val.y, -val.z);
}

vec3 operator+(const vec3& left, const vec3& right)
{
    return vec3(left.x + right.x, left.y + right.y, left.z + right.z);
}

vec3 operator-(const vec3& left, const vec3& right)
{
    return vec3(left.x - right.x, left.y - right.y, left.z - right.z);
}

vec3 operator*(const vec3& left, float right)
{
    return vec3(left.x * right, left.y * right, left.z * right);
}

vec3 operator*(const float left, vec3& right)
{
    return vec3(left * right.x, left * right.y, left * right.z);
}

vec3 operator/(const vec3& left, float right)
{
    return vec3(left.x / right, left.y / right, left.z / right);
}

bool capsule_intersection_test(const vec3& origin, const vec3& direction, const vec3& pa, const vec3& pb, float radius, float& distance)
{
    vec3 ba = pb - pa;
    vec3 oa = origin - pa;
    float baba = ba.dot_product(ba);
    float bard = ba.dot_product(direction);
    float baoa = ba.dot_product(oa);
    float rdoa = direction.dot_product(oa);
    float oaoa = oa.dot_product(oa);
    float a = baba - bard * bard;
    float b = baba * rdoa - baoa * bard;
    float c = baba * oaoa - baoa * baoa - radius * radius * baba;
    float h = b * b - a * c;
    if (h >= 0.0) {
        float t = (-b - sqrt(h)) / a;
        float y = baoa + t * bard;
        // body
        if (y > 0.0 && y < baba) {
            distance = t;
            return true;
        }
        // caps
        vec3 oc = (y <= 0.0) ? oa : origin - pb;
        b = direction.dot_product(oc);
        c = oc.dot_product(oc) - radius * radius;
        h = b * b - c;
        if (h > 0.0) {
            distance = -b - sqrt(h);
            return true;
        }
    }
    return false;
}

// compute normal
vec3 capsule_normal(const vec3& intersection_point, const vec3& a, const vec3& b, float radius)
{
    vec3  ba = b - a;
    vec3  pa = intersection_point - a;
    float h = clamp(0.0f, 1.0f, pa.dot_product(ba) / ba.dot_product(ba));
    return (pa - h*ba) / radius;
}

// float sphere4_intersection_test( in vec3 ro, in vec3 rd, in float ra )
// {
//     float r2 = ra*ra;
//     vec3 d2 = rd*rd; vec3 d3 = d2*rd;
//     vec3 o2 = ro*ro; vec3 o3 = o2*ro;
//     float ka = 1.0/dot(d2,d2);
//     float k3 = ka* dot(ro,d3);
//     float k2 = ka* dot(o2,d2);
//     float k1 = ka* dot(o3,rd);
//     float k0 = ka*(dot(o2,o2) - r2*r2);
//     float c2 = k2 - k3*k3;
//     float c1 = k1 + 2.0*k3*k3*k3 - 3.0*k3*k2;
//     float c0 = k0 - 3.0*k3*k3*k3*k3 + 6.0*k3*k3*k2 - 4.0*k3*k1;
//     float p = c2*c2 + c0/3.0;
//     float q = c2*c2*c2 - c2*c0 + c1*c1;
//     float h = q*q - p*p*p;
//     if( h<0.0 ) return -1.0; //no intersection
//     float sh = sqrt(h);
//     float s = sign(q+sh)*pow(abs(q+sh),1.0/3.0); // cuberoot
//     float t = sign(q-sh)*pow(abs(q-sh),1.0/3.0); // cuberoot
//     vec2  w = vec2( s+t,s-t );
//     vec2  v = vec2( w.x+c2*4.0, w.y*sqrt(3.0) )*0.5;
//     float r = length(v);
//     return -abs(v.y)/sqrt(r+v.x) - c1/r - k3;
// }

// vec3 sph4Normal( in vec3 pos )
// {
//     return normalize( pos*pos*pos );
// }

// sphere of size ra centered at point ce
bool sphere_intersection_test(const vec3& origin, const vec3& direction, const vec3& sphere_center, float sphere_radius, float& distance)
{
    vec3 origin_center = origin - sphere_center;
    float b = origin_center.dot_product(direction);
    float c = origin_center.dot_product(origin_center) - sphere_radius * sphere_radius;
    float h = b * b - c;
    if (h < 0.0) {
        return false;
    }
    h = std::sqrt(h);
    distance = -b - h;
    return true;
}

bool plane_intersection_test(const vec3& origin, const vec3& direction, const vec3& plane_pos, const vec3& plane_normal, float& distance)
{
    float dist = -((origin.dot_product(plane_normal) + sqrt(plane_pos.dot_product(plane_pos))) / direction.dot_product(plane_normal));
    if (dist > 0) {
        distance = dist;
        return true;
    }
    return false;
}


struct plane_type
{
    plane_type(const vec3& pos, const vec3& norm)
        : position{pos}
        , normal{norm}
    {}

    vec3 position;
    vec3 normal;
};

struct sphere_type
{
    sphere_type(const vec3& pos, float r)
        : position{pos}
        , radius{r}
    {}

    vec3 position;
    float radius;
};

struct capsule_type
{
    capsule_type(const vec3& p1, const vec3& p2, float r)
        : point1{p1}
        , point2{p2}
        , radius{r}
    {}

    vec3 point1;
    vec3 point2;
    float radius;
};

enum geometry_type
{
    plane,
    sphere,
    capsule
};

struct primitive
{
    primitive(const sphere_type& sphere)
        : sphere{sphere}
        , type{geometry_type::sphere}
    {}

    primitive(const plane_type& plane)
        : plane{plane}
        , type{geometry_type::plane}
    {}

    primitive(const capsule_type& capsule)
        : capsule{capsule}
        , type{geometry_type::capsule}
    {}

    geometry_type       type;
    vec3                color;
    union
    {
        plane_type      plane;
        sphere_type     sphere;
        capsule_type    capsule;
    };
};

struct light
{
    vec3 position;
    vec3 normal;
    float radius;
};

struct scene
{
    int32_t skip_index = -1;
    std::vector<primitive>  pimitives;
    std::vector<light>      lights;
};

vec3 reflect(const vec3& direction, const vec3& normal)
{
    return direction - normal * 2.0 * direction.dot_product(normal);
}

int raycast(const scene& s, const vec3& origin, const vec3& direction, float& distance, vec3& normal, vec3& intersection)
{
    bool current_test_result;
    float current_test_distance;
    distance = 9e21;
    int i = 0;
    int ret = -1;
    for (auto p: s.pimitives) {
        if (s.skip_index == i) {
            i++;
            continue;
        }
        switch (p.type) {
        case geometry_type::plane:
            current_test_result = plane_intersection_test(origin, direction, p.plane.position, p.plane.normal, current_test_distance);
            if (current_test_result && current_test_distance < distance && current_test_distance >= 0.0f) {
                distance = current_test_distance;
                intersection = origin + direction * distance;
                normal = p.plane.normal;
                ret = i;
            }
            break;
        case geometry_type::sphere:
            current_test_result = sphere_intersection_test(origin, direction, p.sphere.position, p.sphere.radius, current_test_distance);
            if (current_test_result && current_test_distance < distance && current_test_distance >= 0.0f) {
                distance = current_test_distance;
                intersection = origin + direction * distance;
                normal = (intersection - p.sphere.position).normalize_self();
                ret = i;
            }
            break;
        case geometry_type::capsule:
            current_test_result = capsule_intersection_test(origin, direction, p.capsule.point1, p.capsule.point2, p.capsule.radius, current_test_distance);
            if (current_test_result && current_test_distance < distance && current_test_distance >= 0.0f) {
                distance = current_test_distance;
                intersection = origin + direction * distance;
                normal = capsule_normal(intersection, p.capsule.point1, p.capsule.point2, p.capsule.radius);
                ret = i;
            }
            break;
        }
        i++;
    }

    return ret;
}

float frand(float min, float max)
{
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
}

float mix(float a, float b, float coef)
{
    return a * (1.0f - coef) + b * coef;
}

int main() {
    Timer t;
    pixel_storage_uchar3 img(1280, 720);

    scene scene;

    scene.pimitives.emplace_back(plane_type(vec3(0.0, 0.0, -3.0), vec3(0.0, 0.0, 1.0)));
    scene.pimitives.back().color = vec3(0, 0, 1);
    scene.pimitives.emplace_back(sphere_type(vec3(-2.0, 6.0, 1.0), 1.25));
    scene.pimitives.back().color = vec3(1, 0, 0);
    scene.pimitives.emplace_back(capsule_type(vec3(-4.0, 7.0, 0.0), vec3(4.0, 7.5, -0.5), 0.5));
    scene.pimitives.back().color = vec3(0, 1, 0);
    scene.pimitives.emplace_back(sphere_type(vec3(3.0, 6.0, 1.0), 1.0));
    scene.pimitives.back().color = vec3(0, 1, 1);

    scene.pimitives.emplace_back(sphere_type(vec3(2.0, -14.0, 1.3), 2.4));
    scene.pimitives.back().color = vec3(0, 1, 1);



    for (int i = 0; i < 10; i++) {
        scene.pimitives.emplace_back(sphere_type(vec3(frand(-30.0, 30.0), frand(10.0, 30.0), frand(0.0, 20.0)), frand(0.1, 3.0)));
        scene.pimitives.back().color = vec3(frand(0.0, 1.0), frand(0.0, 1.0), frand(0.0, 1.0));
    }

    vec3 origin(0, -5, 2);

    vec3 light_pos(-5, -5, 5);
    vec3 light_dir(1, 1, -1);
    light_dir.normalize();
    vec3 light = -light_dir;
    

    vec3 light_color(0.9, 0.9, 1.0);

    float ratio = 1280.0 / 720.0;
    float dx = (1.0 / 1280.0) * ratio;
    float dy = 1.0 / 720.0;

    vec3 fog_color(0.4, 0.6, 0.4);

    for (int y = 0; y < img.get_rows(); y++) {
        byte* data = static_cast<byte*>(static_cast<void*>(img.get_row_ptr(y)));
        float z_p = static_cast<float>(360 - y) * dy;
        for (int x = 0; x < img.get_columns(); x++) {
            vec3 direction(static_cast<float>(x - 640) * dx, 1.0, z_p);
            direction.normalize_self();

            float dist;
            vec3 norm;
            vec3 intersect;

            scene.skip_index = -1;
            int index = raycast(scene, origin, direction, dist, norm, intersect);
            if (index == -1) {
                data[0] = 255;
                data[1] = 255;
                data[2] = 255;
            } else {
                auto& p = scene.pimitives[index];
                float fog_dist = clamp(0.0f, 250.0f, dist) / 250.0f;
                vec3 color(
                    mix(p.color.x, fog_color.x, fog_dist),
                    mix(p.color.y, fog_color.y, fog_dist),
                    mix(p.color.z, fog_color.z, fog_dist));

                float diffuse_light = clamp(0.0f, 1.0f, light.dot_product(norm) * 0.5f + 0.5f) * 0.5f + 0.1f;
                vec3 reflected = reflect(direction, norm);
                float specular_light = max(0.0f, reflected.dot_product(light));
                specular_light *= specular_light;
                specular_light *= specular_light;
                specular_light *= specular_light;
                specular_light *= specular_light;

                specular_light = clamp(0.0f, 0.7f, specular_light);

                float dist2;
                vec3 norm2;
                vec3 intersect2;

                scene.skip_index = index;
                int ind = raycast(scene, intersect, light, dist2, norm2, intersect2);
                if (ind != -1) {
                    diffuse_light *= 0.6;
                    specular_light *= 0.04;
                }



                float r = clamp(0.0f, 1.0f, (diffuse_light * color.x + specular_light * light_color.x));
                float g = clamp(0.0f, 1.0f, (diffuse_light * color.y + specular_light * light_color.y));
                float b = clamp(0.0f, 1.0f, (diffuse_light * color.z + specular_light * light_color.z));
                data[0] = static_cast<byte>(pow(b, 0.45) * 255.0);;
                data[1] = static_cast<byte>(pow(g, 0.45) * 255.0);;
                data[2] = static_cast<byte>(pow(r, 0.45) * 255.0);;
            }
            data += img.get_bytes_per_cell();
        }
    }

    std::cout << "Time elapsed: " << t.elapsed() << '\n';
    bitmap_save_to_file(img, "img.bmp");

    std::cout << "Time elapsed: " << t.elapsed() << '\n';
}
