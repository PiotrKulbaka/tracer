#pragma once

#include <core/math/vec4.hpp>
#include <core/math/vec3.hpp>
#include <utility>

namespace green::core
{

template <typename T>
class alignas(alignof(T) * 4) basic_mat4
{
public:
    using value_type = T;

public:
                    basic_mat4() = default;
    explicit        basic_mat4(T xx_yy_zz_ww) noexcept;
    explicit        basic_mat4(const basic_vec4<T>& x, const basic_vec4<T>& y, const basic_vec4<T>& z, const basic_vec4<T>& w) noexcept;
    explicit        basic_mat4(T xx, T xy, T xz, T xw, T yx, T yy, T yz, T yw, T zx, T zy, T zz, T zw, T wx, T wy, T wz, T ww) noexcept;

    basic_mat4&     operator+=(const basic_mat4& val) noexcept;
    basic_mat4&     operator-=(const basic_mat4& val) noexcept;
    basic_mat4&     operator*=(const basic_mat4& val) noexcept;
    template <typename S>
    basic_mat4&     operator*=(S val) noexcept;
    template <typename S>
    basic_mat4&     operator/=(S val) noexcept;

    static basic_mat4   perspective(T fov, T aspect, T near, T far) noexcept;

public:
    basic_vec4<T>   x, y, z, w;
}; /* class mat4 */



/* basic_mat4::basic_mat4 */
template <typename T>
inline basic_mat4<T>::basic_mat4(const basic_vec4<T>& x, const basic_vec4<T>& y, const basic_vec4<T>& z, const basic_vec4<T>& w) noexcept
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{}

/* basic_mat4::basic_mat4 */
template <typename T>
inline basic_mat4<T>::basic_mat4(T xx_yy_zz_ww) noexcept
    : x(xx_yy_zz_ww, 0.0, 0.0, 0.0)
    , y(0.0, xx_yy_zz_ww, 0.0, 0.0)
    , z(0.0, 0.0, xx_yy_zz_ww, 0.0)
    , w(0.0, 0.0, 0.0, xx_yy_zz_ww)
{}

/* basic_mat4::basic_mat4 */
template <typename T>
inline basic_mat4<T>::basic_mat4(T xx, T xy, T xz, T xw, T yx, T yy, T yz, T yw, T zx, T zy, T zz, T zw, T wx, T wy, T wz, T ww) noexcept
    : x(xx, xy, xz, xw)
    , y(yx, yy, yz, yw)
    , z(zx, zy, zz, zw)
    , w(wx, wy, wz, ww)
{}

/* basic_mat4::operator+= */
template <typename T>
basic_mat4<T>& basic_mat4<T>::operator+=(const basic_mat4<T>& val) noexcept
{
    x += val.x;
    y += val.y;
    z += val.z;
    w += val.w;
    return *this;
}

/* basic_mat4::operator-= */
template <typename T>
basic_mat4<T>& basic_mat4<T>::operator-=(const basic_mat4<T>& val) noexcept
{
    x -= val.x;
    y -= val.y;
    z -= val.z;
    w -= val.w;
    return *this;
}

/* basic_mat4::operator*= */
template <typename T>
basic_mat4<T>& basic_mat4<T>::operator*=(const basic_mat4<T>& val) noexcept
{
    basic_mat4<T> mat(
        // first row
        x.x * val.x.x + x.y * val.y.x + x.z * val.z.x + x.w * val.w.x,
        x.x * val.x.y + x.y * val.y.y + x.z * val.z.y + x.w * val.w.y,
        x.x * val.x.z + x.y * val.y.z + x.z * val.z.z + x.w * val.w.z,
        x.x * val.x.w + x.y * val.y.w + x.z * val.z.w + x.w * val.w.w,
        // second row
        y.x * val.x.x + y.y * val.y.x + y.z * val.z.x + y.w * val.w.x,
        y.x * val.x.y + y.y * val.y.y + y.z * val.z.y + y.w * val.w.y,
        y.x * val.x.z + y.y * val.y.z + y.z * val.z.z + y.w * val.w.z,
        y.x * val.x.w + y.y * val.y.w + y.z * val.z.w + y.w * val.w.w,
        // third row
        z.x * val.x.x + z.y * val.y.x + z.z * val.z.x + z.w * val.w.x,
        z.x * val.x.y + z.y * val.y.y + z.z * val.z.y + z.w * val.w.y,
        z.x * val.x.z + z.y * val.y.z + z.z * val.z.z + z.w * val.w.z,
        z.x * val.x.w + z.y * val.y.w + z.z * val.z.w + z.w * val.w.w,
        // last row
        w.x * val.x.x + w.y * val.y.x + w.z * val.z.x + w.w * val.w.x,
        w.x * val.x.y + w.y * val.y.y + w.z * val.z.y + w.w * val.w.y,
        w.x * val.x.z + w.y * val.y.z + w.z * val.z.z + w.w * val.w.z,
        w.x * val.x.w + w.y * val.y.w + w.z * val.z.w + w.w * val.w.w
    );
    *this = mat;
    return *this;
}

/* basic_mat4::operator*= */
template <typename T>
template <typename S>
basic_mat4<T>& basic_mat4<T>::operator*=(S val) noexcept
{
    x *= val;
    y *= val;
    z *= val;
    w *= val;
    return *this;
}

/* basic_mat4::operator/= */
template <typename T>
template <typename S>
basic_mat4<T>& basic_mat4<T>::operator/=(S val) noexcept
{
    x /= val;
    y /= val;
    z /= val;
    w /= val;
    return *this;
}

/* static basic_mat4::perspective */
template <typename T>
basic_mat4<T> basic_mat4<T>::perspective(T fov, T aspect, T near, T far) noexcept
{
    auto range = near - far;
    auto half_tan = math::tan(fov / 2.0);
    return basic_mat4<T>(
        1.0 / (half_tan * aspect), 0.0, 0.0, 0.0,
        0.0, 1.0 / half_tan, 0.0, 0.0,
        0.0, 0.0, (-near - far) / range, 2.0 * far * near / range,
        0.0, 0.0, 1.0, 0.0);
}



/* operator+ */
template <typename T>
basic_mat4<T> operator+(const basic_mat4<T>& a, const basic_mat4<T>& b) noexcept
{
    return basic_mat4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

/* operator- */
template <typename T>
basic_mat4<T> operator-(const basic_mat4<T>& a, const basic_mat4<T>& b) noexcept
{
    return basic_mat4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

/* operator* */
template <typename T>
basic_mat4<T> operator*(const basic_mat4<T>& a, const basic_mat4<T>& b) noexcept
{
    return basic_mat4<T>(
        // first row
        a.x.x * b.x.x + a.x.y * b.y.x + a.x.z * b.z.x + a.x.w * b.w.x,
        a.x.x * b.x.y + a.x.y * b.y.y + a.x.z * b.z.y + a.x.w * b.w.y,
        a.x.x * b.x.z + a.x.y * b.y.z + a.x.z * b.z.z + a.x.w * b.w.z,
        a.x.x * b.x.w + a.x.y * b.y.w + a.x.z * b.z.w + a.x.w * b.w.w,
        // second row
        a.y.x * b.x.x + a.y.y * b.y.x + a.y.z * b.z.x + a.y.w * b.w.x,
        a.y.x * b.x.y + a.y.y * b.y.y + a.y.z * b.z.y + a.y.w * b.w.y,
        a.y.x * b.x.z + a.y.y * b.y.z + a.y.z * b.z.z + a.y.w * b.w.z,
        a.y.x * b.x.w + a.y.y * b.y.w + a.y.z * b.z.w + a.y.w * b.w.w,
        // third row
        a.z.x * b.x.x + a.z.y * b.y.x + a.z.z * b.z.x + a.z.w * b.w.x,
        a.z.x * b.x.y + a.z.y * b.y.y + a.z.z * b.z.y + a.z.w * b.w.y,
        a.z.x * b.x.z + a.z.y * b.y.z + a.z.z * b.z.z + a.z.w * b.w.z,
        a.z.x * b.x.w + a.z.y * b.y.w + a.z.z * b.z.w + a.z.w * b.w.w,
        // last row
        a.w.x * b.x.x + a.w.y * b.y.x + a.w.z * b.z.x + a.w.w * b.w.x,
        a.w.x * b.x.y + a.w.y * b.y.y + a.w.z * b.z.y + a.w.w * b.w.y,
        a.w.x * b.x.z + a.w.y * b.y.z + a.w.z * b.z.z + a.w.w * b.w.z,
        a.w.x * b.x.w + a.w.y * b.y.w + a.w.z * b.z.w + a.w.w * b.w.w
    );
}

/* operator* */
template <typename T>
basic_vec4<T> operator*(const basic_vec4<T>& v, const basic_mat4<T>& m) noexcept
{
    return basic_vec4<T>(
        v.x * m.x.x + v.y * m.y.x + v.z * m.z.x + v.w * m.w.x,
        v.x * m.x.y + v.y * m.y.y + v.z * m.z.y + v.w * m.w.y,
        v.x * m.x.z + v.y * m.y.z + v.z * m.z.z + v.w * m.w.z,
        v.x * m.x.w + v.y * m.y.w + v.z * m.z.w + v.w * m.w.w
    );
}

/* operator* */
template <typename T>
basic_vec4<T> operator*(const basic_mat4<T>& m, const basic_vec4<T>& v) noexcept
{
    return basic_vec4<T>(
        v.x * m.x.x + v.y * m.x.y + v.z * m.x.z + v.w * m.x.w,
        v.x * m.y.x + v.y * m.y.y + v.z * m.y.z + v.w * m.y.w,
        v.x * m.z.x + v.y * m.z.y + v.z * m.z.z + v.w * m.z.w,
        v.x * m.w.x + v.y * m.w.y + v.z * m.w.z + v.w * m.w.w
    );
}

/* operator* */
template <typename T>
basic_vec3<T> operator*(const basic_mat4<T>& m, const basic_vec3<T>& v) noexcept
{
    auto scale = m.w.x * v.x + m.w.y * v.y + m.w.z * v.z + m.w.w;
    if (scale == 1.0) {
        return basic_vec3<T>(
            m.x.x * v.x + m.x.y * v.y + m.x.z * v.z + m.x.w,
            m.y.x * v.x + m.y.y * v.y + m.y.z * v.z + m.y.w,
            m.z.x * v.x + m.z.y * v.y + m.z.z * v.z + m.z.w
        );
    } else if (scale == 0.0) {
        return basic_vec3<T>(0.0);
    }
    return basic_vec3<T>(
        (m.x.x * v.x + m.x.y * v.y + m.x.z * v.z + m.x.w) / scale,
        (m.y.x * v.x + m.y.y * v.y + m.y.z * v.z + m.y.w) / scale,
        (m.z.x * v.x + m.z.y * v.y + m.z.z * v.z + m.z.w) / scale
    );
}

/* operator* */
template <typename T, typename S>
basic_mat4<T> operator*(const basic_mat4<T>& m, S s)
{
    return basic_mat4<T>(m.x * s, m.y * s, m.z * s, m.w * s);
}

/* operator* */
template <typename T, typename S>
basic_mat4<T> operator*(S s, const basic_mat4<T>& m)
{
    return basic_mat4<T>(m.x * s, m.y * s, m.z * s, m.w * s);
}

/* operator/ */
template <typename T, typename S>
basic_mat4<T> operator/(const basic_mat4<T>& m, S s)
{
    return m * (1.0 / s);
}

/* operator/ */
template <typename T, typename S>
basic_mat4<T> operator/(S s, const basic_mat4<T>& m)
{
    return basic_mat4<T>(s / m.x, s / m.y, s / m.z, s / m.w);
}

using fmat4 = basic_mat4<float>;
using dmat4 = basic_mat4<double>;

} /* namespace green::core */
