#pragma once

template <class V3, typename S>
bool ray_sphere_intersection_test(const V3& ro, const V3& rd, const V3& sph_center, S sph_radius, S& near, S& far)
{
    V3 oc = ro - sph_center;
    S b = oc.dot(rd);
    S c = oc.dot(oc) - sph_radius * sph_radius;
    S h = b * b - c;
    if (h < 0.0) {
        return false;
    }
    h = std::sqrt(h);
    near = -b - h;
    far = -b + h;
    return true;
}

template <class V3, typename S>
bool ray_aabb_intersection_test(const V3& ro, const V3& rd, const V3& aabb_center, const V3& aabb_size, S& near, S& far, V3& near_normal, V3& far_normal)
{
    V3 m = 1.0 / rd; // can precompute if traversing a set of aligned boxes
    V3 n = m * (ro - aabb_center);   // can precompute if traversing a set of aligned boxes
    V3 k = m.abs() * aabb_size;
    V3 t1 = -n - k;
    V3 t2 = -n + k;
    S tN = t1.max();
    S tF = t2.min();
    if (tN > tF || tF <= 0.0) {
        return false;
    }
    near = tN;
    far = tF;
    auto step = [](const V3& edge, const V3& a) {
        return V3(a.x < edge.x ? 0.0 : 1.0, a.y < edge.y ? 0.0 : 1.0, a.z < edge.z ? 0.0 : 1.0);
    };
    if (tN > 0.0) {
        near_normal = step(V3(tN), t1);
        far_normal = step(t2, V3(tF));
    } else {
        near_normal = step(t2, V3(tF));
        far_normal = step(V3(tN), t1);
    }
    near_normal *= -rd.sign();
    return true;
}


template <class V3, typename S>
bool ray_cylinder_intersection_test(const V3& ro, const V3& rd, const V3& pa, const V3& pb, S ra, S& near, S& far, V3& near_normal)
{
    V3 ba = pb - pa;
    V3 oc = ro - pa;
    S baba = ba.dot(ba);
    S bard = ba.dot(rd);
    S baoc = ba.dot(oc);
    S k2 = baba - bard * bard;
    S k1 = baba * oc.dot(rd) - baoc * bard;
    S k0 = baba * oc.dot(oc) - baoc * baoc - ra * ra * baba;
    S h = k1 * k1 - k2 * k0;
    if (h < 0.0) {
        return false;
    }
    h = std::sqrt(h);
    S t = (-k1 - h) / k2;
    // body
    S y = baoc + t * bard;
    if (y > 0.0 && y < baba) {
        near = t;
        near_normal = (oc + t * rd - ba * y / baba) / ra;
        return true;
    }
    // caps
    auto sign = [](S s) {
        return s < 0.0 ? -1.0 : (s > 0.0 ? 1.0 : 0.0);
    };
    t = (((y < 0.0) ? 0.0 : baba) - baoc) / bard;
    if (std::abs(k1 + k2 * t) < h) {
        near = t;
        near_normal = ba * sign(y) / std::sqrt(baba);
        return true;
    }
    return false;
}

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
