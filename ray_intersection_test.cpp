#include "ray_intersection_test.hpp"

namespace green::core
{

#define PLANE_TEST_CALC_COMMON_RET_FALSE() \
    float b = rd.dot(norm); \
    if (b == 0.0) { \
        return false;   \
    }   \
    float a = -ro.dot(norm) + w;    \
    float dist = a / b; \
    if (dist < 0.0) {   \
        return false;   \
    }

/* ray_pane_intersection_test */
bool ray_pane_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& norm, float w)
{
    PLANE_TEST_CALC_COMMON_RET_FALSE();
    return true;
}

/* ray_pane_intersection_test */
bool ray_pane_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& norm, float w, float& near)
{
    PLANE_TEST_CALC_COMMON_RET_FALSE();
    near = dist;
    return true;
}

/* ray_pane_intersection_test */
bool ray_pane_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& norm, float w, float& near, fvec3& near_norm)
{
    PLANE_TEST_CALC_COMMON_RET_FALSE();
    near = dist;
    near_norm = b < 0.0 ? -norm : norm;
    return true;
}

#define SPH_TEST_CALC_COMMON_RET_FALSE() \
    fvec3 oc = ro - sph_pos; \
    float b = oc.dot(rd); \
    float c = oc.dot(oc) - sph_r * sph_r; \
    float h = b * b - c; \
    if (h < 0.0) { \
        return false; \
    }

/* ray_sphere_intersection_test */
bool ray_sphere_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& sph_pos, float sph_r)
{
    SPH_TEST_CALC_COMMON_RET_FALSE();
    return true;
}

/* ray_sphere_intersection_test */
bool ray_sphere_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& sph_pos, float sph_r, float& near, float& far)
{
    SPH_TEST_CALC_COMMON_RET_FALSE();
    h = green::core::math::sqrt(h);
    float _n = -b - h;
    if (_n < 0.0) {
        return false;
    }
    near = _n;
    far = -b + h;
    return true;
}

/* ray_sphere_intersection_test */
bool ray_sphere_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& sph_pos, float sph_r, float& near, float& far, fvec3& near_norm, fvec3& far_norm)
{
    SPH_TEST_CALC_COMMON_RET_FALSE();
    h = green::core::math::sqrt(h);
    float _n = -b - h;
    if (_n < 0.0) {
        return false;
    }
    near = _n;
    far = -b + h;
    near_norm = (ro + rd * near - sph_pos).normalize_self();
    far_norm = (ro + rd * far - sph_pos).normalize_self();
    return true;
}

#define AABB_TEST_CALC_COMMON_RET_FALSE()   \
    fvec3 m = 1.0 / rd;    \
    fvec3 n = m * (ro - aabb_pos); \
    fvec3 k = m.abs() * aabb_size; \
    fvec3 t1 = -n - k; \
    fvec3 t2 = -n + k; \
    float tN = t1.max();    \
    float tF = t2.min();    \
    if (tN > tF || tN < 0.0 || tF <= 0.0) { \
        return false;   \
    }

bool ray_aabb_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& aabb_pos, const fvec3& aabb_size)
{
    AABB_TEST_CALC_COMMON_RET_FALSE();
    return true;
}

bool ray_aabb_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& aabb_pos, const fvec3& aabb_size, float& near, float& far)
{
    AABB_TEST_CALC_COMMON_RET_FALSE();
    near = tN;
    far = tF;
    return true;
}

bool ray_aabb_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& aabb_pos, const fvec3& aabb_size, float& near, float& far, fvec3& near_norm, fvec3& far_norm)
{
    AABB_TEST_CALC_COMMON_RET_FALSE();
    near = tN;
    far = tF;
    auto step = [](const fvec3& edge, const fvec3& a) {
        return fvec3(a.x < edge.x ? 0.0 : 1.0, a.y < edge.y ? 0.0 : 1.0, a.z < edge.z ? 0.0 : 1.0);
    };
    if (tN > 0.0) {
        near_norm = step(fvec3(tN), t1);
        far_norm = step(t2, fvec3(tF));
    } else {
        near_norm = step(t2, fvec3(tF));
        far_norm = step(fvec3(tN), t1);
    }
    near_norm *= -rd.sign();
    return true;
}

#define AABB_TEST_CALC_COMMON() \
    fvec3 ba = pb - pa;    \
    fvec3 oa = ro - pa;    \
    float baba = ba.dot(ba);    \
    float bard = ba.dot(rd);    \
    float baoa = ba.dot(oa);    \
    float rdoa = rd.dot(oa);    \
    float oaoa = oa.dot(oa);    \
    float a = baba - bard * bard;   \
    float b = baba * rdoa - baoa * bard;    \
    float c = baba * oaoa - baoa * baoa - cap_r * cap_r * baba; \
    float h = b * b - a * c;

bool ray_capsule_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& pa, const fvec3& pb, float cap_r)
{
    AABB_TEST_CALC_COMMON();
    if (h >= 0.0) {
        float sqrth = sqrt(h);
        float t = (-b - sqrth) / a;
        float y = baoa + t * bard;
        // body
        if (y > 0.0 && y < baba) {
            if (t < 0.0) {
                return false;
            }
            return true;
        }
        // caps
        fvec3 oc = (y <= 0.0) ? oa : ro - pb;
        b = rd.dot(oc);
        c = oc.dot(oc) - cap_r * cap_r;
        h = b * b - c;
        if (h >= 0.0) {
            h = sqrt(h);
            float _n = -b - h;
            if (_n < 0.0) {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool ray_capsule_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& pa, const fvec3& pb, float cap_r, float& near, float& far)
{
    AABB_TEST_CALC_COMMON();
    if (h >= 0.0) {
        float sqrth = sqrt(h);
        float t = (-b - sqrth) / a;
        float y = baoa + t * bard;
        // body
        if (y > 0.0 && y < baba) {
            if (t < 0.0) {
                return false;
            }
            near = t;
            far = (-b + sqrth) / a;
            return true;
        }
        // caps
        fvec3 oc = (y <= 0.0) ? oa : ro - pb;
        b = rd.dot(oc);
        c = oc.dot(oc) - cap_r * cap_r;
        h = b * b - c;
        if (h >= 0.0) {
            h = sqrt(h);
            float _n = -b - h;
            if (_n < 0.0) {
                return false;
            }
            near = _n;
            far = -b + h;
            return true;
        }
    }
    return false;
}

bool ray_capsule_intersection_test(const fvec3& ro, const fvec3& rd, const fvec3& pa, const fvec3& pb, float cap_r, float& near, float& far, fvec3& near_norm, fvec3& far_norm)
{
    AABB_TEST_CALC_COMMON();
    if (h >= 0.0) {
        float sqrth = sqrt(h);
        float t = (-b - sqrth) / a;
        float y = baoa + t * bard;
        // body
        if (y > 0.0 && y < baba) {
            if (t < 0.0) {
                return false;
            }
            near = t;
            far = (-b + sqrth) / a;
            goto calc_norm;
        }
        // caps
        fvec3 oc = (y <= 0.0) ? oa : ro - pb;
        b = rd.dot(oc);
        c = oc.dot(oc) - cap_r * cap_r;
        h = b * b - c;
        if (h >= 0.0) {
            h = sqrt(h);
            float _n = -b - h;
            if (_n < 0.0) {
                return false;
            }
            near = _n;
            far = -b + h;
            goto calc_norm;
        }
    }
    return false;

calc_norm:
    fvec3  p_a = (ro + rd * near) - pa;
    float hh = math::clamp(p_a.dot(ba) / ba.dot(ba), 0.0f, 1.0f);
    near_norm = (p_a - hh * ba) / cap_r;
    p_a = (ro + rd * far) - pa;
    hh = math::clamp(p_a.dot(ba) / ba.dot(ba), 0.0f, 1.0f);
    far_norm = (p_a - hh * ba) / cap_r;
    
    return true;
}

} /* namespace green::core */
