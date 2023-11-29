#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>

#include "thread_pool.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <core/pixel_format.hpp>
#include <core/matrix.hpp>
#include <core/math.hpp>
#include <core/timer.hpp>
#include <engine/camera.hpp>
#include "ray_intersection_test.hpp"

using namespace green::core;
using namespace green::core::math;

using pixel_storage_fvec3 = basic_matrix<fvec3>;

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

bool bitmap_save_to_file(const pixel_storage_fvec3& image, const std::string& filename)
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
            *dst++ = static_cast<byte>(data->b * 255.0);
            *dst++ = static_cast<byte>(data->g * 255.0);
            *dst++ = static_cast<byte>(data->r * 255.0);
            data++;
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

pixel_storage_fvec3 bitmap_load_from_file(const std::string& filename) {
    
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
    
    pixel_storage_fvec3 image(full.info.width, full.info.height);


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
        fvec3* data = image.get_row_ptr(full.info.height - i - 1);
        for( int j = 0; j < full.info.width; j++ ) {
            /* Read one pixel data from file */
            if( !file.read( reinterpret_cast<char*>(buffer), bmpPxSize ) ) {
                std::cout << "image::load_bmp() error: reading error (read data)" << std::endl;
            }
            data->r = static_cast<float>(buffer[2]) / 255.0;
            data->g = static_cast<float>(buffer[1]) / 255.0;
            data->b = static_cast<float>(buffer[0]) / 255.0;
            data++;
            //this->data.insert( this->data.end(), bufferTo, bufferTo + bmpPxSize );
        }
        if( dataPadding ) {
            file.seekg(dataPadding, std::ios_base::cur);
        }
    }
    return image;
}


inline fvec3 intersection_point(const fvec3& origin, const fvec3& direction, float distance)
{
    return origin + direction * distance;
}

// compute normal
fvec3 capsule_normal(const fvec3& intersection_point, const fvec3& a, const fvec3& b, float radius)
{
    fvec3  ba = b - a;
    fvec3  pa = intersection_point - a;
    float h = clamp(pa.dot(ba) / ba.dot(ba), 0.0f, 1.0f);
    return (pa - h*ba) / radius;
}

fvec3 sphere_intersection_normal(const fvec3& intersection, const fvec3& sph_center)
{
    return (intersection - sph_center).normalize_self();
}

bool plane_intersection_test(const fvec3& origin, const fvec3& direction, const fvec3& plane_pos, const fvec3& plane_normal, float& distance)
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
    aabb_type(const fvec3& center, const fvec3& size)
        : center(center)
        , size(size)
    {}

    fvec3 center;
    fvec3 size;
};

struct plane_type
{
    plane_type(const fvec3& pos, const fvec3& norm)
        : position{pos}
        , normal{norm}
    {}

    fvec3 position;
    fvec3 normal;
};

struct sphere_type
{
    sphere_type(const fvec3& pos, float r)
        : position{pos}
        , radius{r}
    {}

    fvec3 position;
    float radius;
};

struct capsule_type
{
    capsule_type(const fvec3& p1, const fvec3& p2, float r)
        : point1{p1}
        , point2{p2}
        , radius{r}
    {}

    fvec3 point1;
    fvec3 point2;
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
        : type(geometry_type::sphere)
        , sphere(sphere)
    {}

    primitive(const plane_type& plane)
        : type(geometry_type::plane)
        , plane(plane)
    {}

    primitive(const capsule_type& capsule)
        : type(geometry_type::capsule)
        , capsule(capsule)
    {}

    primitive(const aabb_type& aabb)
        : type(geometry_type::aabb)
        , aabb(aabb)
    {}

    geometry_type       type;
    //                  цвет
    fvec3              diffuse;
    //                  Отраженная часть (reflection)
    //                  Преломленная часть (refraction)
    float               specular;
    //                  шероховатость
    float               roughness;

    float               glowing = 0.0f;

    bool                transparent = false;
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
    fvec3                  light_dir;
    fvec3                  light_color;
    pixel_storage_fvec3    sky;
};

fvec3 getSky(const scene& s, const fvec3& rd)
{
    float u = (math::atan2(rd.x, rd.y) / pi) * 0.5 + 0.5;
    float v = (math::asin(-rd.z) / pi) + 0.5;
    int32_t col = static_cast<int32_t>(u * static_cast<float>(s.sky.get_columns()));
    int32_t row = static_cast<int32_t>(v * static_cast<float>(s.sky.get_rows()));
    col = math::clamp(col, 0, s.sky.get_columns() - 1);
    row = math::clamp(row, 0, s.sky.get_rows() - 1);
	return *s.sky.get_cell_ptr(row, col);
}

fvec3 reflect(const fvec3& direction, const fvec3& normal)
{
    return direction - normal * 2.0 * direction.dot(normal);
}

fvec3 refract(const fvec3& direction, const fvec3& normal, float eta)
{
    float dotndir = normal.dot(direction);
    float k = 1.0 - eta * eta * (1.0f - dotndir * dotndir);
    if (k < 0.0) {
        return fvec3(0.0, 0.0, 0.0);
    }
    return eta * direction - (eta * dotndir + std::sqrt(k)) * normal;
}

int raycast(const scene& s, int skip_index, const fvec3& origin, const fvec3& direction, float& near, float& far, fvec3& normal_near, fvec3& normal_far)
{
    bool cur_result;
    float dist_near;
    float dist_far;
    fvec3 far_normal;
    fvec3 near_normal;
    near = 9e99;
    int i = 0;
    int ret = -1;
    for (auto p: s.primitives) {
        if (skip_index == i) {
            i++;
            continue;
        }
        switch (p.type) {
        case geometry_type::plane:
            cur_result = ray_pane_intersection_test(origin, direction, p.plane.normal, p.plane.position.dot(p.plane.normal), dist_near, near_normal);
            break;
        case geometry_type::sphere:
            cur_result = ray_sphere_intersection_test(origin, direction, p.sphere.position, p.sphere.radius, dist_near, dist_far, near_normal, far_normal);
            break;
        case geometry_type::capsule:
            cur_result = ray_capsule_intersection_test(origin, direction, p.capsule.point1, p.capsule.point2, p.capsule.radius, dist_near, dist_far, near_normal, far_normal);
            break;
        case geometry_type::aabb:
            cur_result = ray_aabb_intersection_test(origin, direction, p.aabb.center, p.aabb.size, dist_near, dist_far, near_normal, far_normal);
            break;
        }
        if (cur_result && dist_near < near) {
            near = dist_near;
            far = dist_far;
            normal_near = near_normal;
            normal_far = far_normal;
            ret = i;
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

void simple_rendering(scene& s, const fvec3& origin, pixel_storage_fvec3& img)
{
    float ratio = static_cast<float>(img.get_columns()) / img.get_rows();
    float dx = (1.0 / img.get_columns()) * ratio;
    float dy = 1.0 / img.get_rows();
    float half_width = img.get_columns() / 2.0;
    float half_height = img.get_rows() / 2.0;

    fvec3 light = -s.light_dir;
    fvec3 far_n;
    float far;

    for (int y = 0; y < img.get_rows(); y++) {
        fvec3* data = img.get_row_ptr(y);
        float z_p = static_cast<float>(half_height - y) * dy;
        for (int x = 0; x < img.get_columns(); x++) {
            fvec3 direction(static_cast<float>(x - half_width) * dx, 1.0, z_p);
            direction.normalize_self();

            float dist;
            fvec3 norm;
            fvec3 intersect;

            s.skip_index = -1;
            int index = raycast(s, s.skip_index, origin, direction, dist, far, norm, far_n);
            if (index == -1) {
                *data = getSky(s, direction);
            } else {
                auto& p = s.primitives[index];
                auto color = p.diffuse;

                float diffuse_light = clamp(light.dot(norm) * 0.5f + 0.5f, 0.0f, 1.0f) * 0.5f + 0.1f;
                fvec3 reflected = reflect(direction, norm);
                float specular_light = max(0.0f, reflected.dot(light));
                specular_light *= specular_light;
                specular_light *= specular_light;
                specular_light *= specular_light;
                specular_light *= specular_light;

                specular_light = clamp(specular_light, 0.0f, 0.7f);

                float dist2;
                fvec3 norm2;

                s.skip_index = index;
                intersect = intersection_point(origin, direction, dist);
                int ind = raycast(s, s.skip_index, intersect, light, dist2, far, norm2, far_n);
                if (ind != -1) {
                    diffuse_light *= 0.6;
                    specular_light *= 0.04;
                }

                data->r = clamp(diffuse_light * color.x + specular_light * s.light_color.x, 0.0f, 1.0f);
                data->g = clamp(diffuse_light * color.y + specular_light * s.light_color.y, 0.0f, 1.0f);
                data->b = clamp(diffuse_light * color.z + specular_light * s.light_color.z, 0.0f, 1.0f);
            }
            data++;
        }
    }
}


void normal_rendering(scene& s, const fvec3& origin, pixel_storage_fvec3& img)
{
    float ratio = static_cast<float>(img.get_columns()) / img.get_rows();
    float dx = (1.0 / img.get_columns()) * ratio;
    float dy = 1.0 / img.get_rows();
    float half_width = img.get_columns() / 2.0;
    float half_height = img.get_rows() / 2.0;

    fvec3 far_n;
    float far;

    for (int y = 0; y < img.get_rows(); y++) {
        fvec3* data = img.get_row_ptr(y);
        float z_p = static_cast<float>(half_height - y) * dy;
        for (int x = 0; x < img.get_columns(); x++) {
            fvec3 direction(static_cast<float>(x - half_width) * dx, 1.0, z_p);
            direction.normalize_self();

            float dist;
            fvec3 norm;

            s.skip_index = -1;
            int index = raycast(s, s.skip_index, origin, direction, dist, far, norm, far_n);
            *data = index == -1 ? fvec3(0.5, 0.5, 1.0) : norm * 0.5 + 0.5;
            data++;
        }
    }
}

void fast_simple_rendering(scene& s, const fvec3& origin, pixel_storage_fvec3& img)
{
    float ratio = static_cast<float>(img.get_columns()) / img.get_rows();
    float dx = (1.0 / img.get_columns()) * ratio;
    float dy = 1.0 / img.get_rows();
    float half_width = img.get_columns() / 2.0;
    float half_height = img.get_rows() / 2.0;

    fvec3 light = -s.light_dir;
    fvec3 far_n;
    float far;

    for (int y = 0; y < img.get_rows(); y++) {
        fvec3* data = img.get_row_ptr(y);
        float z_p = static_cast<float>(half_height - y) * dy;
        for (int x = 0; x < img.get_columns(); x++) {
            fvec3 direction(static_cast<float>(x - half_width) * dx, 1.0, z_p);
            direction.normalize_self();

            float dist;
            fvec3 norm;
            fvec3 intersect;

            s.skip_index = -1;
            int index = raycast(s, s.skip_index, origin, direction, dist, far, norm, far_n);
            if (index == -1) {
                *data = getSky(s, direction);
            } else {
                auto& p = s.primitives[index];
                auto color = p.diffuse;
                
                float diffuse_light = (light.dot(norm) >= 0.0f ? 1.0f : 0.5f);
                fvec3 reflected = reflect(direction, norm);
                float specular_light = reflected.dot(light) >= 0.8f ? 0.8f : 0.0f;

                s.skip_index = index;
                intersect = intersection_point(origin, direction, dist);
                int ind = raycast(s, s.skip_index, intersect, light, dist, far, norm, far_n);
                if (ind != -1) {
                    diffuse_light *= 0.6;
                    specular_light *= 0.04;
                }

                *data = fvec3(diffuse_light * color.x + specular_light * s.light_color.x,
                    diffuse_light * color.y + specular_light * s.light_color.y,
                    diffuse_light * color.z + specular_light * s.light_color.z).clamp_self(0.0, 1.0);
            }
            data++;
        }
    }
}

bool random_statement(float p)
{
    return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) < p;
}

fvec3 random_on_sphere()
{
    return fvec3(frand(-1.0, 1.0), frand(-1.0, 1.0), frand(-1.0, 1.0)).normalize_self();
}

fvec3 raytrace(const scene& s, int& skip_index, fvec3& origin, fvec3& direction)
{
    float dist;
    float dist_far;
    fvec3 norm;
    fvec3 norm_far;
    fvec3 intersect;

    int index = raycast(s, skip_index, origin, direction, dist, dist_far, norm, norm_far);
    skip_index = index;
    if (index == -1) {
        return getSky(s, direction);
    } else {
        intersect = intersection_point(origin, direction, dist);
        auto& p = s.primitives[index];
        fvec3 diffuse = p.diffuse;
        float specular = p.specular;
        float roughness = p.roughness;

        if (random_statement(p.glowing)) {
            index = -1;
            return diffuse;
        }

        if (p.transparent && random_statement(-direction.dot(norm))) {
            direction = -refract(direction, norm_far, -1.0);
            origin = intersection_point(origin, direction, dist_far);
            return fvec3(1.0, 1.0, 1.0);
        }

        fvec3 reflected = reflect(direction, norm);

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

        fvec3 r = random_on_sphere();
        fvec3 diffuse_rand = (r * r.dot(norm)).normalize_self();
        origin = intersect;
        direction = fvec3(
            mix(reflected.x, diffuse_rand.x, roughness),
            mix(reflected.y, diffuse_rand.y, roughness),
            mix(reflected.z, diffuse_rand.z, roughness));
        return diffuse;
    }
}

void physic_rendering(scene& s, const fvec3& origin_, pixel_storage_fvec3& img)
{
    float ratio = static_cast<float>(img.get_columns()) / img.get_rows();
    float dx = (1.0 / img.get_columns()) * ratio;
    float dy = 1.0 / img.get_rows();
    float half_width = img.get_columns() / 2.0;
    float half_height = img.get_rows() / 2.0;

    for (int y = 0; y < img.get_rows(); y++) {
        fvec3* data = img.get_row_ptr(y);
        float z_p = static_cast<float>(half_height - y) * dy;
        for (int x = 0; x < img.get_columns(); x++) {
            fvec3 direction_(static_cast<float>(x - half_width) * dx, 1.0, z_p);
            direction_.normalize_self();

            constexpr int steps = 8;

            fvec3 color(0.0, 0.0, 0.0);

            constexpr int iters = 32 * 1;

            for (int k = 0; k < iters; k++) {
                fvec3 origin = origin_;
                int skip_index = -1;
                fvec3 col(1.0, 1.0, 1.0);
                fvec3 direction = direction_;

                int i;
                for (i = 0; i < steps; i++) {
                    fvec3 cl = raytrace(s, skip_index, origin, direction);
                    col = col * cl;
                    if (skip_index == -1) {
                        break;
                    }
                }
                if (i == steps) {
                    col = fvec3(0.0, 0.0, 0.0);
                } else if (i == 0) {
                    color = col * iters;
                    break;
                }

                color = color + col;
            }

            color *= 1.0 / iters;

            *data = color.clamp(0.0, 1.0);
            data++;
        }
    }
}

void render_pass_line(const scene& s, const fvec3& origin_, float z_p, float dx, int width, fvec3* dst)
{
    int half_width = width / 2;
    for (int x = 0; x < width; x++) {
        fvec3 direction_(static_cast<float>(x - half_width) * dx, 1.0, z_p);
        direction_.normalize_self();

        constexpr int steps = 8;

        fvec3 color(0.0, 0.0, 0.0);

        constexpr int iters = 16 * 16;

        for (int k = 0; k < iters; k++) {
            fvec3 origin = origin_;
            int skip_index = -1;
            fvec3 col(1.0, 1.0, 1.0);
            fvec3 direction = direction_;

            int i;
            for (i = 0; i < steps; i++) {
                fvec3 cl = raytrace(s, skip_index, origin, direction);
                col = col * cl;
                if (skip_index == -1) {
                    break;
                }
            }
            if (i == steps) {
                col = fvec3(0.0, 0.0, 0.0);
            } else if (i == 0) {
                color = col * iters;
                break;
            }

            color = color + col;
        }

        color *= 1.0 / iters;

        *dst = color.clamp(0.0, 1.0);
        dst++;
    }
}

void render_pass(scene& s, const fvec3& origin_, pixel_storage_fvec3& img)
{
    float ratio = static_cast<float>(img.get_columns()) / img.get_rows();
    float dx = (1.0 / img.get_columns()) * ratio;
    float dy = 1.0 / img.get_rows();
    float half_height = img.get_rows() / 2.0;

    thread_pool scheduler(8);

    for (int y = 0; y < img.get_rows(); y++) {
        fvec3* data = img.get_row_ptr(y);
        float z_p = static_cast<float>(half_height - y) * dy;
        //render_pass_line(s, origin_, z_p, dx, img.get_columns(), data);
        scheduler.enqueue(render_pass_line, s, origin_, z_p, dx, img.get_columns(), data);
    }

    //scheduler.join();
}


void gamma_correction_pass(pixel_storage_fvec3& image)
{
    for (int32_t y = 0; y < image.get_rows(); y++) {
        for (int32_t x = 0; x < image.get_columns(); x++) {
            //pow(clamp(diffuse_light * color.x + specular_light * s.light_color.x, 0.0f, 1.0f), 0.45);
        }
    }
}

int main()
{
    pixel_storage_fvec3 img(12800, 7200);

    scene scene;

    green::camera cam(fvec3(0, -22, 2), fvec3(0, 1, 0), fvec3(0, 0, 1));
    cam.set_perspective_projection(pi / 3.0, static_cast<float>(img.get_columns()) / img.get_rows(), 0.1, 1000);

    float w = 0.1;
    fvec3 spos(3.0, 10.0, 5.0);
    fvec3 sscale(2.0, 3.0, 1.5);

    // H
    scene.primitives.emplace_back(capsule_type(fvec3(-1.0, -1.0, -1.0), fvec3(-1.0,  1.0, -1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3(-1.0, -1.0, -1.0), fvec3(1.0, -1.0, -1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3(-1.0,  1.0, -1.0), fvec3(1.0,  1.0, -1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3(1.0, -1.0, -1.0), fvec3(1.0,  1.0, -1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);

    scene.primitives.emplace_back(capsule_type(fvec3(-1.0, -1.0, -1.0), fvec3(-1.0,  -1.0, 1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3(-1.0,  1.0, -1.0), fvec3(-1.0, 1.0, 1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3( 1.0, -1.0, -1.0), fvec3(1.0,  -1.0, 1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3(1.0,  1.0, -1.0), fvec3(1.0,  1.0, 1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);

    scene.primitives.emplace_back(capsule_type(fvec3(-1.0, -1.0, 1.0), fvec3(-1.0,  1.0, 1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3(-1.0, -1.0, 1.0), fvec3(1.0, -1.0, 1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3(-1.0,  1.0, 1.0), fvec3(1.0,  1.0, 1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.emplace_back(capsule_type(fvec3(1.0, -1.0, 1.0), fvec3(1.0,  1.0, 1.0), w));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);

    for (auto& p: scene.primitives) {
        p.capsule.point1 *= sscale;
        p.capsule.point2 *= sscale;
        p.capsule.point1 += spos;
        p.capsule.point2 += spos;
    }

    // scene.primitives.emplace_back(plane_type(fvec3(0.0, 0.0, -5.0), fvec3(0.0, 0.0, 1.0)));
    // scene.primitives.back().diffuse = fvec3(0.2, 0.4, 1);
    // scene.primitives.back().specular = 0.2;
    // scene.primitives.back().roughness = 0.9f;
    // scene.primitives.back().glowing = 0.6;
    scene.primitives.emplace_back(sphere_type(fvec3(-2.0, 6.0, 1.0), 1.25));
    scene.primitives.back().diffuse = fvec3(1, 0, 0);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.5f;
    scene.primitives.emplace_back(capsule_type(fvec3(-4.0, 7.0, 0.0), fvec3(4.0, 7.5, -0.5), 1.15));
    scene.primitives.back().diffuse = fvec3(0, 1, 0);
    scene.primitives.back().specular = 0.5;
    scene.primitives.back().roughness = 1.0f;
    scene.primitives.emplace_back(sphere_type(fvec3(3.0, 6.0, 1.0), 1.0));
    scene.primitives.back().diffuse = fvec3(0, 1, 1);
    scene.primitives.back().specular = 0.5;
    scene.primitives.back().roughness = 1.0f;

    scene.primitives.emplace_back(sphere_type(fvec3(-1.0, 3.0, -3.0), 1.25));
    scene.primitives.back().diffuse = fvec3(1, 1, 0);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.0f;

    scene.primitives.emplace_back(sphere_type(fvec3(2.0, -14.0, 1.3), 2.4));
    scene.primitives.back().diffuse = fvec3(0, 1, 1);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.0f;
    scene.primitives.back().transparent = true;

    scene.light_dir = fvec3(1, 1, -1).normalize_self();
    scene.light_color = fvec3(0.9, 0.9, 1.0);


    scene.primitives.emplace_back(aabb_type(fvec3(2.0, -3.0, -1.0), fvec3(1.0, 1.5, 1.0)));
    scene.primitives.back().diffuse = fvec3(0, 0.5, 1);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.4f;

    scene.primitives.emplace_back(aabb_type(fvec3(-5.0, -3.0, 5.0), fvec3(3.0, 1.5, 1.2)));
    scene.primitives.back().diffuse = fvec3(0.0, 0.5, 1);
    scene.primitives.back().specular = 1.0;
    scene.primitives.back().roughness = 0.4f;


    fvec3 origin(0, -22, 2);

    timer t;

    int iw, ih, ic;
    unsigned char* imgData = stbi_load("UV_checker_Map_byValle.jpg", &iw, &ih, &ic, 0);

    if (imgData == NULL) {
        std::cout << "Cannot load texture" << std::endl;
        return 0;
    } else {
        std::cout << "iw: " << iw << " ih: " << ih << " ic: " << ic << std::endl;
    }

    pixel_storage_fvec3 img_sky = pixel_storage_fvec3(iw, ih);
    unsigned char* id = imgData;
    fvec3* px = img_sky.get_storage_ptr();
    for (int y = 0; y < ih * iw; y++) {
        px->r = static_cast<float>(id[0]) / 255.0;
        px->g = static_cast<float>(id[1]) / 255.0;
        px->b = static_cast<float>(id[2]) / 255.0;
        id += ic;
        px++;
    }

     bitmap_save_to_file(img_sky, "my_image.bmp");


    stbi_image_free(imgData);

    scene.sky = img_sky;
    std::cout <<"loaded" << std::endl;

    render_pass(scene, origin, img);
    std::cout <<"rendered" << std::endl;

    std::cout << "Time elapsed: " << t.get_elapsed_sec() << '\n';
    bitmap_save_to_file(img, "img.bmp");

    std::cout << "Time elapsed: " << t.get_elapsed_sec() << '\n';
}
