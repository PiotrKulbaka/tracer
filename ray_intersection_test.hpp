#pragma once

#include <core/math.hpp>

namespace green::core
{

bool ray_pane_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& norm, float w);
bool ray_pane_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& norm, float w, float& near);
bool ray_pane_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& norm, float w, float& near, fvec3& near_norm);

bool ray_sphere_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& sph_pos, float sph_r);
bool ray_sphere_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& sph_pos, float sph_r, float& near, float& far);
bool ray_sphere_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& sph_pos, float sph_r, float& near, float& far, fvec3& near_norm, fvec3& far_norm);

bool ray_aabb_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& aabb_pos, const fvec3& aabb_size);
bool ray_aabb_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& aabb_pos, const fvec3& aabb_size, float& near, float& far);
bool ray_aabb_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& aabb_pos, const fvec3& aabb_size, float& near, float& far, fvec3& near_norm, fvec3& far_norm);

bool ray_capsule_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& pa, const fvec3& pb, float cap_r);
bool ray_capsule_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& pa, const fvec3& pb, float cap_r, float& near, float& far);
bool ray_capsule_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& pa, const fvec3& pb, float cap_r, float& near, float& far, fvec3& near_norm, fvec3& far_norm);




// template <class V3, typename S>
// bool ray_cylinder_intersection_test(const V3& ro, const V3& rd, const V3& pa, const V3& pb, S ra, S& near, S& far, V3& near_normal)
// {
//     V3 ba = pb - pa;
//     V3 oc = ro - pa;
//     S baba = ba.dot(ba);
//     S bard = ba.dot(rd);
//     S baoc = ba.dot(oc);
//     S k2 = baba - bard * bard;
//     S k1 = baba * oc.dot(rd) - baoc * bard;
//     S k0 = baba * oc.dot(oc) - baoc * baoc - ra * ra * baba;
//     S h = k1 * k1 - k2 * k0;
//     if (h < 0.0) {
//         return false;
//     }
//     h = green::core::math::sqrt(h);
//     S t = (-k1 - h) / k2;
//     // body
//     S y = baoc + t * bard;
//     if (y > 0.0 && y < baba) {
//         near = t;
//         near_normal = (oc + t * rd - ba * y / baba) / ra;
//         return true;
//     }
//     // caps
//     t = (((y < 0.0) ? 0.0 : baba) - baoc) / bard;
//     if (green::core::math::abs(k1 + k2 * t) < h) {
//         near = t;
//         near_normal = ba * green::core::math::sign(y) / green::core::math::sqrt(baba);
//         return true;
//     }
//     return false;
// }

// bool intersectPlane(const Vec3f &n, const Vec3f &p0, const Vec3f &l0, const Vec3f &l, float &t) 
// { 
//     // assuming vectors are all normalized
//     float denom = dotProduct(n, l); 
//     if (denom > 1e-6) { 
//         Vec3f p0l0 = p0 - l0; 
//         t = dotProduct(p0l0, n) / denom; 
//         return (t >= 0); 
//     } 
 
//     return false; 
// }

// bool intersectDisk(const Vec3f &n, const Vec3f &p0, const float &radius, const Vec3f &l0, const Vec3 &l) 
// { 
//     float t = 0; 
//     if (intersectPlane(n, p0, l0, l, t)) { 
//         Vec3f p = l0 + l * t; 
//         Vec3f v = p - p0; 
//         float d2 = dot(v, v); 
//         return (sqrtf(d2) <= radius); 
//         // or you can use the following optimisation (and precompute radius^2)
//         // return d2 <= radius2; // where radius2 = radius * radius
//      } 
 
//      return false; 
// } 

} /* namespace green::core */
