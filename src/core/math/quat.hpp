#pragma once

#include <core/math/vec3.hpp>
#include <core/math/vec4.hpp>
#include <core/math/mat4.hpp>

namespace green::core
{

template <typename T>
class alignas(alignof(T) * 4) basic_quat
{
public:
    using value_type = T;

public:

                    basic_quat() = default;
                    basic_quat(T x, T y, T z, T w) noexcept;
                    /* n - normalized vector. angle_w - angle in radians */
                    basic_quat(const basic_vec3<T>& n, T radian_w) noexcept;

    basic_quat &    operator*=(const basic_quat& q) noexcept;

    basic_quat      normalize() const noexcept;
    basic_quat &    normalize_self() noexcept;

    basic_mat4<T>   to_mat4() const noexcept;

public:
    T               x, y, z, w;
}; /* class quat */



/* basic_quat::basic_quat */
template <typename T>
inline basic_quat<T>::basic_quat(T x, T y, T z, T w) noexcept
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

/* basic_quat::basic_quat */
template <typename T>
inline basic_quat<T>::basic_quat(const basic_vec3<T>& n, T radian_w) noexcept
{
    auto half_angle = radian_w / 2.0;
    auto sin = math::sin(half_angle);
    auto cos = math::cos(half_angle);
    x = n.x * sin;
    y = n.y * sin;
    z = n.z * sin;
    w = cos;
}

template <typename T>
inline basic_quat<T>& basic_quat<T>::operator*=(const basic_quat<T>& q) noexcept
{
    T qx = w * q.x + x * q.w + y * q.z - z * q.y;
    T qy = w * q.y - x * q.z + y * q.w + z * q.x;
    T qz = w * q.z + x * q.y - y * q.x + z * q.w;
    T qw = w * q.w - x * q.x - y * q.y - z * q.z;
    x = qx;
    y = qy;
    z = qz;
    w = qw;
    return *this;
}

template <typename T>
inline basic_quat<T> basic_quat<T>::normalize() const noexcept
{
    T len = math::sqrt(x * x + y * y + z * z + w * w);
    if (len != 0.0 ) {
        return basic_quat<T>(x / len, y / len, z / len, w / len);
    }
    return basic_quat<T>(0.0, 0.0, 0.0, 1.0);
}

template <typename T>
inline basic_quat<T>& basic_quat<T>::normalize_self() noexcept
{
    T len = math::sqrt(x * x + y * y + z * z + w * w);
    if (len != 0.0 ) {
        x /= len;
        y /= len;
        z /= len;
        w /= len;
    } else {
        w = 1.0;
    }
    return *this;
}

template <typename T>
basic_mat4<T> basic_quat<T>::to_mat4() const noexcept
{
    T x2 = x + x;
    T y2 = y + y;
    T z2 = z + z;
    T xx = x * x2;
    T xy = x * y2;
    T xz = x * z2;
    T yy = y * y2;
    T yz = y * z2;
    T zz = z * z2;
    T wx = w * x2;
    T wy = w * y2;
    T wz = w * z2;
    return basic_mat4<T>(
        1.0 - (yy + zz), xy - wz, xz + wy, 0.0,
        xy + wz, 1.0 - (xx + zz), yz - wx, 0.0,
        xz - wy, yz + wx, 1.0 - (xx + yy), 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}



/* operator- */
template<typename T>
inline basic_quat<T> operator-(const basic_quat<T>& q) noexcept
{
    return basic_quat<T>(-q.x, -q.y, -q.z, q.w);
}

/* operator* */
template<typename T>
inline basic_quat<T> operator*(const basic_quat<T>& a, const basic_quat<T>& b) noexcept
{
    return basic_quat<T>(
        a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    );
}

/* operator* */
template<typename T>
basic_vec3<T> operator*(const basic_quat<T>& q, const basic_vec3<T>& v) noexcept
{
    T xxzz = q.x * q.x - q.z * q.z;
    T wwyy = q.w * q.w - q.y * q.y;
    T xy2 = q.x * q.y * 2.0;
    T xz2 = q.x * q.z * 2.0;
    T xw2 = q.x * q.w * 2.0;
    T yw2 = q.y * q.w * 2.0;
    T yz2 = q.y * q.z * 2.0;
    T zw2 = q.z * q.w * 2.0;
    return basic_vec3<T>( 
        v.x * (xxzz + wwyy) + v.y * (xy2 + zw2) + v.z * (xz2 - yw2),
        v.x * (xy2 - zw2) + v.y * (q.y * q.y + q.w * q.w - q.x * q.x - q.z * q.z) + v.z * (yz2 + xw2),
        v.x * (xz2 + yw2) + v.y * (yz2 - xw2) + v.z * (wwyy - xxzz)
    );
}

/* operator* */
template<typename T>
basic_vec3<T> operator*(const basic_vec3<T>& v, const basic_quat<T>& q)
{
    return q * v;
}


using fquat = basic_quat<float>;
using dquat = basic_quat<double>;

} /* namespace green::core */
