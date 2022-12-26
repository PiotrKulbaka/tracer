#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "pixel_format.hpp"
#include "matrix.hpp"
#include "vec3.hpp"
#include "ray_intersection_test.hpp"

using float3 = basic_vec3<float>;
using pixel_storage_float3 = basic_matrix<float, 3>;

#include <chrono> // для функций из std::chrono

class timer
{
private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

    std::chrono::time_point<clock_t> m_beg;

public:
    timer() : m_beg(clock_t::now()) {}
    void reset() { m_beg = clock_t::now(); }
    double elapsed() const { return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count(); }
};

float clamp(float min, float max, float val)
{
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

// float clamp(float min, float max, float val)
// {
//     if (val < min) {
//         return min;
//     }
//     if (val > max) {
//         return max;
//     }
//     return val;
// }

float max(float a, float b)
{
    return a > b ? a : b;
}

// float max(float a, float b)
// {
//     return a > b ? a : b;
// }

template <typename TDst, typename TSrc>
inline TDst pointer_cast(TSrc src)
{
    return static_cast<TDst>(static_cast<void*>(src));
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

bool bitmap_save_to_file(const pixel_storage_float3& image, const std::string& filename)
{
    bitmap_header full;

    /* Set bitmap file header */
    full.header.offset = static_cast<dword>(sizeof(full));
    full.header.size = full.header.offset + image.get_columns() * 3 * image.get_rows();
    /* Set bitmap info header */
    full.info.width = image.get_columns();
    full.info.height = image.get_rows();
    full.info.bitCount = static_cast<word>(3 * 8);
    full.info.imageSize = image.get_columns() * 3 * image.get_rows();

    std::fstream file;
    file.open(filename, std::ios_base::out | std::ios_base::binary);

    /* Write file and info headers */
    if (!file.write(static_cast<char*>(static_cast<void*>(&full)), sizeof(full))) {
        std::cout << "image::save_to_file() error: writing error (full)" << std::endl;
        return false;
    }

    auto buf_size = image.get_columns() * 3;
    byte* buffer = new byte[buf_size];
    bool ret = true;

    /* Write bitmap data */
    for (int i = 0; i < image.get_rows(); i++) {
        auto* data = image.get_row_ptr(image.get_rows() - i - 1);
        auto* dst = buffer;
        for (int j = 0; j < image.get_columns(); j++) {
            *dst++ = static_cast<byte>(data[2] * 255.0f);
            *dst++ = static_cast<byte>(data[1] * 255.0f);
            *dst++ = static_cast<byte>(data[0] * 255.0f);
            data += 3;
        }
        if (!file.write(static_cast<char*>(static_cast<void*>(buffer)), buf_size)) {
            std::cout << "image::save_to_file() error: writing error" << std::endl;
            ret = false;
            goto go_ret;
        }
    }

go_ret:
    delete[] buffer;
    return ret;
}

pixel_storage_float3 bitmap_load_from_file(const std::string& filename) {
    
    bitmap_header full;

    std::fstream file;
    file.open(filename, std::ios_base::in | std::ios_base::binary);
    
    /* Check for correct data */
    if (!file.read( reinterpret_cast<char*>(&full), sizeof(full))) {
        //return false;
    }
    
    if( full.info.size != sizeof(bitmap_info_header) ) {
        std::cout << "image::load_bmp() error: info.size != sizeof(info)" << std::endl;
        //return false;
    }
    /* check supported formats */
    if( auto b = full.info.bitCount; !(b == 24) ) {
        std::cout << "image::load_bmp() error: unsupported format info.bitCount" << std::endl;
        //return false;
    }
    if( full.info.width <= 0 || full.info.height <= 0 ) {
        std::cout << "image::load_bmp() error: wrong image size" << std::endl;
        //return false;
    }
    
    pixel_storage_float3 image(full.info.width, full.info.height);


    //reserve( info.width, info.height, fmt );

    int bmpPxSize = full.info.bitCount >> 3;
    // int pxSize = get_bpp() >> 3;
    int stride = ((full.info.bitCount * full.info.width + 31) & ~31) >> 3;
    int rowSize = ((full.info.bitCount * full.info.width + 7) & ~7) >> 3;
    int dataPadding = stride - rowSize;
    // assert( dataPadding == 0 );
     byte buffer[256];
    
    file.seekg(full.header.offset, std::ios_base::beg);
    for (int i = 0; i < full.info.height; i++) {
        /* Set pointer for copying data */
        float *data = image.get_row_ptr(full.info.height - i - 1);
        for( int j = 0; j < full.info.width; j++ ) {
            /* Read one pixel data from file */
            if( !file.read( reinterpret_cast<char*>(buffer), bmpPxSize ) ) {
                std::cout << "image::load_bmp() error: reading error (read data)" << std::endl;
            }
            data[0] = static_cast<float>(buffer[2]) / 255.0;
            data[1] = static_cast<float>(buffer[1]) / 255.0;
            data[2] = static_cast<float>(buffer[0]) / 255.0;
            data += 3;
            //this->data.insert( this->data.end(), bufferTo, bufferTo + bmpPxSize );
        }
        if( dataPadding ) {
            file.seekg(dataPadding, std::ios_base::cur);
        }
    }
    return image;
}


inline float3 intersection_point(const float3& origin, const float3& direction, float distance)
{
    return origin + direction * distance;
}

bool capsule_intersection_test(const float3& origin, const float3& direction, const float3& pa, const float3& pb, float radius, float& near, float& far)
{
    float3 ba = pb - pa;
    float3 oa = origin - pa;
    float baba = ba.dot(ba);
    float bard = ba.dot(direction);
    float baoa = ba.dot(oa);
    float rdoa = direction.dot(oa);
    float oaoa = oa.dot(oa);
    float a = baba - bard * bard;
    float b = baba * rdoa - baoa * bard;
    float c = baba * oaoa - baoa * baoa - radius * radius * baba;
    float h = b * b - a * c;
    if (h >= 0.0) {
        float t = (-b - sqrt(h)) / a;
        float y = baoa + t * bard;
        // body
        if (y > 0.0 && y < baba) {
            near = t;
            far = t;
            return true;
        }
        // caps
        float3 oc = (y <= 0.0) ? oa : origin - pb;
        b = direction.dot(oc);
        c = oc.dot(oc) - radius * radius;
        h = b * b - c;
        if (h > 0.0) {
            h = sqrt(h);
            near = -b - h;
            far = -b + h;
            return true;
        }
    }
    return false;
}

// compute normal
float3 capsule_normal(const float3& intersection_point, const float3& a, const float3& b, float radius)
{
    float3  ba = b - a;
    float3  pa = intersection_point - a;
    float h = clamp(0.0f, 1.0f, pa.dot(ba) / ba.dot(ba));
    return (pa - h*ba) / radius;
}

float3 sphere_intersection_normal(const float3& intersection, const float3& sph_center)
{
    return (intersection - sph_center).normalize_self();
}

bool plane_intersection_test(const float3& origin, const float3& direction, const float3& plane_pos, const float3& plane_normal, float& distance)
{
    float dist = -((origin.dot(plane_normal) + sqrt(plane_pos.dot(plane_pos))) / direction.dot(plane_normal));
    if (dist > 0) {
        distance = dist;
        return true;
    }
    return false;
}


struct aabb_type
{
    aabb_type(const float3& center, const float3& size)
        : center(center)
        , size(size)
    {}

    float3 center;
    float3 size;
};

struct plane_type
{
    plane_type(const float3& pos, const float3& norm)
        : position{pos}
        , normal{norm}
    {}

    float3 position;
    float3 normal;
};

struct sphere_type
{
    sphere_type(const float3& pos, float r)
        : position{pos}
        , radius{r}
    {}

    float3 position;
    float radius;
};

struct capsule_type
{
    capsule_type(const float3& p1, const float3& p2, float r)
        : point1{p1}
        , point2{p2}
        , radius{r}
    {}

    float3 point1;
    float3 point2;
    float radius;
};

enum geometry_type
{
    plane,
    sphere,
    capsule,
    aabb
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

    primitive(const aabb_type& aabb)
        : aabb{aabb}
        , type{geometry_type::aabb}
    {}

    geometry_type       type;
    //                  цвет
    float3                diffuse;
    //                  Отраженная часть (reflection)
    //                  Преломленная часть (refraction)
    float               specular;
    //                  шероховатость
    float               roughness;

    float               glowing = 0.0f;
    union
    {
        plane_type      plane;
        sphere_type     sphere;
        capsule_type    capsule;
        aabb_type       aabb;
    };
};

struct scene
{
    int32_t                 skip_index = -1;
    std::vector<primitive>  primitives;
    float3                  light_dir;
    float3                  light_color;
    pixel_storage_float3    sky;
};

float3 getSky(const scene& s, const float3& rd)
{
    float u = (std::atan2(rd.x, rd.y) / 3.14159265358979) * 0.5 + 0.5;
    float v = -(std::asin(rd.z) / 3.14159265358979) * 0.5 + 0.5;
    int32_t col = static_cast<int32_t>(u * static_cast<float>(s.sky.get_columns()));
    int32_t row = static_cast<int32_t>(v * static_cast<float>(s.sky.get_rows()));
    float* c = s.sky.get_cell_ptr(row, col);
    float3 color(c[0], c[1], c[2]);
	return color;
}

float3 reflect(const float3& direction, const float3& normal)
{
    return direction - normal * 2.0 * direction.dot(normal);
}

float3 refract(const float3& direction, const float3& normal, float eta)
{
    float dotndir = normal.dot(direction);
    float k = 1.0 - eta * eta * (1.0f - dotndir * dotndir);
    if (k < 0.0) {
        return float3(0.0, 0.0, 0.0);
    }
    return eta * direction - (eta * dotndir + std::sqrt(k)) * normal;
}

int raycast(const scene& s, const float3& origin, const float3& direction, float& near, float3& normal, float3& intersection)
{
    bool current_test_result;
    float current_test_distance;
    float current_test_far_distance;
    float3 far_normal;
    near = 9e21;
    int i = 0;
    int ret = -1;
    for (auto p: s.primitives) {
        if (s.skip_index == i) {
            i++;
            continue;
        }
        switch (p.type) {
        case geometry_type::plane:
            current_test_result = plane_intersection_test(origin, direction, p.plane.position, p.plane.normal, current_test_distance);
            if (current_test_result && current_test_distance < near && current_test_distance >= 0.0f) {
                near = current_test_distance;
                intersection = intersection_point(origin, direction, near);
                normal = p.plane.normal;
                ret = i;
            }
            break;
        case geometry_type::sphere:
            current_test_result = ray_sphere_intersection_test(origin, direction, p.sphere.position, p.sphere.radius, current_test_distance, current_test_far_distance);
            if (current_test_result && current_test_distance < near && current_test_distance >= 0.0f) {
                near = current_test_distance;
                intersection = intersection_point(origin, direction, near);
                normal = sphere_intersection_normal(intersection, p.sphere.position);
                ret = i;
            }
            break;
        case geometry_type::capsule:
            current_test_result = capsule_intersection_test(origin, direction, p.capsule.point1, p.capsule.point2, p.capsule.radius, current_test_distance, current_test_far_distance);
            if (current_test_result && current_test_distance < near && current_test_distance >= 0.0f) {
                near = current_test_distance;
                intersection = intersection_point(origin, direction, near);
                normal = capsule_normal(intersection, p.capsule.point1, p.capsule.point2, p.capsule.radius);
                ret = i;
            }
            break;
        case geometry_type::aabb:
            current_test_result = ray_aabb_intersection_test(origin, direction, p.aabb.center, p.aabb.size, current_test_distance, current_test_far_distance, normal, far_normal);
            if (current_test_result && current_test_distance < near && current_test_distance >= 0.0f) {
                near = current_test_distance;
                intersection = intersection_point(origin, direction, near);
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

void simple_rendering(scene& s, const float3& origin, pixel_storage_float3& img)
{
    float ratio = static_cast<float>(img.get_columns()) / img.get_rows();
    float dx = (1.0 / img.get_columns()) * ratio;
    float dy = 1.0 / img.get_rows();
    float half_width = img.get_columns() / 2.0;
    float half_height = img.get_rows() / 2.0;

    float3 light = -s.light_dir;
    float3 light_color = s.light_color;

    for (int y = 0; y < img.get_rows(); y++) {
        float* data = img.get_row_ptr(y);
        float z_p = static_cast<float>(half_height - y) * dy;
        for (int x = 0; x < img.get_columns(); x++) {
            float3 direction(static_cast<float>(x - half_width) * dx, 1.0, z_p);
            direction.normalize_self();

            float dist;
            float3 norm;
            float3 intersect;

            s.skip_index = -1;
            int index = raycast(s, origin, direction, dist, norm, intersect);
            if (index == -1) {
                float3 cl = getSky(s, direction);
                data[0] = cl.x;
                data[1] = cl.y;
                data[2] = cl.z;
            } else {
                auto& p = s.primitives[index];
                auto color = p.diffuse;

                float diffuse_light = clamp(0.0f, 1.0f, light.dot(norm) * 0.5f + 0.5f) * 0.5f + 0.1f;
                float3 reflected = reflect(direction, norm);
                float specular_light = max(0.0f, reflected.dot(light));
                specular_light *= specular_light;
                specular_light *= specular_light;
                specular_light *= specular_light;
                specular_light *= specular_light;

                specular_light = clamp(0.0f, 0.7f, specular_light);

                float dist2;
                float3 norm2;
                float3 intersect2;

                s.skip_index = index;
                int ind = raycast(s, intersect, light, dist2, norm2, intersect2);
                if (ind != -1) {
                    diffuse_light *= 0.6;
                    specular_light *= 0.04;
                }

                data[0] = pow(clamp(0.0f, 1.0f, (diffuse_light * color.x + specular_light * s.light_color.x)), 0.45);
                data[1] = pow(clamp(0.0f, 1.0f, (diffuse_light * color.y + specular_light * s.light_color.y)), 0.45);
                data[2] = pow(clamp(0.0f, 1.0f, (diffuse_light * color.z + specular_light * s.light_color.z)), 0.45);
            }
            data += 3;
        }
    }
}

void fast_simple_rendering(scene& s, const float3& origin, pixel_storage_float3& img)
{
    float ratio = static_cast<float>(img.get_columns()) / img.get_rows();
    float dx = (1.0 / img.get_columns()) * ratio;
    float dy = 1.0 / img.get_rows();
    float half_width = img.get_columns() / 2.0;
    float half_height = img.get_rows() / 2.0;

    float3 light = -s.light_dir;
    float3 light_color = s.light_color;

    for (int y = 0; y < img.get_rows(); y++) {
        float* data = img.get_row_ptr(y);
        float z_p = static_cast<float>(half_height - y) * dy;
        for (int x = 0; x < img.get_columns(); x++) {
            float3 direction(static_cast<float>(x - half_width) * dx, 1.0, z_p);
            direction.normalize_self();

            float dist;
            float3 norm;
            float3 intersect;

            s.skip_index = -1;
            int index = raycast(s, origin, direction, dist, norm, intersect);
            if (index == -1) {
                float3 cl = getSky(s, direction);
                data[0] = cl.x;
                data[1] = cl.y;
                data[2] = cl.z;
            } else {
                auto& p = s.primitives[index];
                auto color = p.diffuse;
                
                float diffuse_light = (light.dot(norm) >= 0.0f ? 1.0f : 0.5f);
                float3 reflected = reflect(direction, norm);
                float specular_light = reflected.dot(light) >= 0.8f ? 0.8f : 0.0f;

                s.skip_index = index;
                int ind = raycast(s, intersect, light, dist, norm, intersect);
                if (ind != -1) {
                    diffuse_light *= 0.6;
                    specular_light *= 0.04;
                }

                data[0] = pow(clamp(0.0f, 1.0f, (diffuse_light * color.x + specular_light * s.light_color.x)), 0.45);
                data[1] = pow(clamp(0.0f, 1.0f, (diffuse_light * color.y + specular_light * s.light_color.y)), 0.45);
                data[2] = pow(clamp(0.0f, 1.0f, (diffuse_light * color.z + specular_light * s.light_color.z)), 0.45);
            }
            data += 3;
        }
    }
}

bool random_statement(float p)
{
    return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) < p;
}

float3 random_on_sphere()
{
    return float3(frand(-1.0, 1.0), frand(-1.0, 1.0), frand(-1.0, 1.0)).normalize_self();
}

float3 raytrace(scene& s, float3& origin, float3& direction)
{
    float dist;
    float3 norm;
    float3 intersect;

    s.skip_index = raycast(s, origin, direction, dist, norm, intersect);
    if (s.skip_index == -1) {
        return getSky(s, direction);
    } else {
        auto& p = s.primitives[s.skip_index];
        float3 diffuse = p.diffuse;
        float specular = p.specular;
        float roughness = p.roughness;

        if (random_statement(p.glowing)) {
            s.skip_index = -1;
            return diffuse;
        }

        float3 reflected = reflect(direction, norm);

        if (random_statement(specular)) {
            float fresnel = 1.0 - std::abs(norm.dot(direction));
            if (random_statement(fresnel * fresnel)) {
                direction = reflected;
                origin = intersect;
                return diffuse;
            }
            origin = intersect;
            direction = reflect(direction, norm);
            return diffuse * specular;
        }

        float3 r = random_on_sphere();
        float3 diffuse_rand = (r * r.dot(norm)).normalize_self();
        origin = intersect;
        direction = float3(
            mix(reflected.x, diffuse_rand.x, roughness),
            mix(reflected.y, diffuse_rand.y, roughness),
            mix(reflected.z, diffuse_rand.z, roughness));
        return diffuse;
    }
}

void physic_rendering(scene& s, const float3& origin_, pixel_storage_float3& img)
{
    float ratio = static_cast<float>(img.get_columns()) / img.get_rows();
    float dx = (1.0 / img.get_columns()) * ratio;
    float dy = 1.0 / img.get_rows();
    float half_width = img.get_columns() / 2.0;
    float half_height = img.get_rows() / 2.0;

    for (int y = 0; y < img.get_rows(); y++) {
        float* data = img.get_row_ptr(y);
        float z_p = static_cast<float>(half_height - y) * dy;
        for (int x = 0; x < img.get_columns(); x++) {
            float3 direction_(static_cast<float>(x - half_width) * dx, 1.0, z_p);
            direction_.normalize_self();

            
            constexpr int steps = 64;

            float3 color(0.0, 0.0, 0.0);

            float coef = 1.0f / 8.0f;

            constexpr int iters = 256 * 8;

            for (int k = 0; k < iters; k++) {
                float3 origin = origin_;
                s.skip_index = -1;
                float3 col(1.0, 1.0, 1.0);
                float3 direction = direction_;

                int i;
                for (i = 0; i < steps; i++) {
                    float3 cl = raytrace(s, origin, direction);
                    col = col * cl;
                    if (s.skip_index == -1) {
                        break;
                    }
                }
                if (i == steps) {
                    col = float3(0.0, 0.0, 0.0);
                }

                color = color + col;
            }

            color *= 1.0 / iters;

            data[0] = pow(clamp(0.0f, 1.0f, color.x), 0.45);
            data[1] = pow(clamp(0.0f, 1.0f, color.y), 0.45);
            data[2] = pow(clamp(0.0f, 1.0f, color.z), 0.45);

            data += 3;
        }
    }
}

int main() {
    
    pixel_storage_float3 img(1280, 720);

    scene scene;


    float w = 0.1;
    float3 spos(3.0, 10.0, 5.0);
    float3 sscale(2.0, 3.0, 1.5);

    // H
    scene.primitives.emplace_back(capsule_type(float3(-1.0, -1.0, -1.0), float3(-1.0,  1.0, -1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3(-1.0, -1.0, -1.0), float3(1.0, -1.0, -1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3(-1.0,  1.0, -1.0), float3(1.0,  1.0, -1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3(1.0, -1.0, -1.0), float3(1.0,  1.0, -1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);

    scene.primitives.emplace_back(capsule_type(float3(-1.0, -1.0, -1.0), float3(-1.0,  -1.0, 1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3(-1.0,  1.0, -1.0), float3(-1.0, 1.0, 1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3( 1.0, -1.0, -1.0), float3(1.0,  -1.0, 1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3(1.0,  1.0, -1.0), float3(1.0,  1.0, 1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);

    scene.primitives.emplace_back(capsule_type(float3(-1.0, -1.0, 1.0), float3(-1.0,  1.0, 1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3(-1.0, -1.0, 1.0), float3(1.0, -1.0, 1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3(-1.0,  1.0, 1.0), float3(1.0,  1.0, 1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(float3(1.0, -1.0, 1.0), float3(1.0,  1.0, 1.0), w));
    scene.primitives.back().diffuse = float3(0, 1, 0);

    for (auto& p: scene.primitives) {
        p.capsule.point1 *= sscale;
        p.capsule.point2 *= sscale;
        p.capsule.point1 += spos;
        p.capsule.point2 += spos;
    }



    // scene.primitives.emplace_back(plane_type(float3(0.0, 0.0, -3.0), float3(0.0, 0.0, 1.0)));
    // scene.primitives.back().diffuse = float3(0.2, 0.4, 1);
    // scene.primitives.back().specular = 0.2;
    // scene.primitives.back().roughness = 0.9f;
    // scene.primitives.back().glowing = 0.6;
    scene.primitives.emplace_back(sphere_type(float3(-2.0, 6.0, 1.0), 1.25));
    scene.primitives.back().diffuse = float3(1, 0, 0);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.5f;
    scene.primitives.emplace_back(capsule_type(float3(-4.0, 7.0, 0.0), float3(4.0, 7.5, -0.5), 1.15));
    scene.primitives.back().diffuse = float3(0, 1, 0);
    scene.primitives.back().specular = 0.5;
    scene.primitives.back().roughness = 1.0f;
    scene.primitives.emplace_back(sphere_type(float3(3.0, 6.0, 1.0), 1.0));
    scene.primitives.back().diffuse = float3(0, 1, 1);
    scene.primitives.back().specular = 0.5;
    scene.primitives.back().roughness = 1.0f;

    scene.primitives.emplace_back(sphere_type(float3(-1.0, 3.0, -3.0), 1.25));
    scene.primitives.back().diffuse = float3(1, 1, 0);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.0f;

    scene.primitives.emplace_back(sphere_type(float3(2.0, -14.0, 1.3), 2.4));
    scene.primitives.back().diffuse = float3(0, 1, 1);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.0f;

    scene.light_dir = float3(1, 1, -1).normalize_self();
    scene.light_color = float3(0.9, 0.9, 1.0);


    scene.primitives.emplace_back(aabb_type(float3(2.0, -3.0, -1.0), float3(1.0, 1.5, 1.0)));
    scene.primitives.back().diffuse = float3(0, 0.5, 1);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.4f;

    scene.primitives.emplace_back(aabb_type(float3(-5.0, -3.0, 5.0), float3(3.0, 1.5, 1.2)));
    scene.primitives.back().diffuse = float3(0.0, 0.5, 1);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.4f;


    float3 origin(0, -15, 2);

    timer t;

    scene.sky = bitmap_load_from_file("panorama.bmp");

    simple_rendering(scene, origin, img);

    std::cout << "Time elapsed: " << t.elapsed() << '\n';
    bitmap_save_to_file(img, "img.bmp");

    std::cout << "Time elapsed: " << t.elapsed() << '\n';
}
