#pragma once

#include <core/math/math_functions.hpp>
#include <core/math/vec3.hpp>

namespace green::core
{

template <typename T>
class alignas(alignof(T) * 4) basic_vec4
{
public:
    using value_type = T;

public:
                    basic_vec4() = default;
                    basic_vec4(T x) noexcept;
                    basic_vec4(T x, T y, T z, T w) noexcept;

    basic_vec4&     operator+=(const basic_vec4& val) noexcept;
    basic_vec4&     operator-=(const basic_vec4& val) noexcept;
    basic_vec4&     operator*=(const basic_vec4& val) noexcept;
    template <typename S>
    basic_vec4&     operator*=(S val) noexcept;
    basic_vec4&     operator/=(const basic_vec4& val) noexcept;
    template <typename S>
    basic_vec4&     operator/=(S val) noexcept;

    const basic_vec3<T>&  vec3() const noexcept;
    basic_vec3<T>&  vec3() noexcept;

public:
    union {
        T x, r;
    };
    union
    {
        T y, g;
    };
    union
    {
        T z, b;
    };
    union
    {
        T w, a;
    };
}; /* class vec4 */



/* basic_vec4::basic_vec4 */
template <typename T>
inline basic_vec4<T>::basic_vec4(T x) noexcept
    : x(x)
    , y(x)
    , z(x)
    , w(x)
{
}

/* basic_vec4::basic_vec4 */
template <typename T>
inline basic_vec4<T>::basic_vec4(T x, T y, T z, T w) noexcept
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

/* basic_vec4::operator+= */
template <typename T>
inline basic_vec4<T>& basic_vec4<T>::operator+=(const basic_vec4<T>& val) noexcept
{
    x += val.x;
    y += val.y;
    z += val.z;
    w += val.w;
    return *this;
}

/* basic_vec4::operator-= */
template <typename T>
inline basic_vec4<T>& basic_vec4<T>::operator-=(const basic_vec4<T>& val) noexcept
{
    x -= val.x;
    y -= val.y;
    z -= val.z;
    w -= val.w;
    return *this;
}

/* basic_vec4::operator*= */
template <typename T>
inline basic_vec4<T>& basic_vec4<T>::operator*=(const basic_vec4<T>& val) noexcept
{
    x *= val.x;
    y *= val.y;
    z *= val.z;
    w *= val.w;
    return *this;
}

/* basic_vec4::operator*= */
template <typename T>
template <typename S>
inline basic_vec4<T>& basic_vec4<T>::operator*=(S val) noexcept
{
    x *= val;
    y *= val;
    z *= val;
    w *= val;
    return *this;
}

/* basic_vec4::operator/= */
template <typename T>
inline basic_vec4<T>& basic_vec4<T>::operator/=(const basic_vec4<T>& val) noexcept
{
    x /= val.x;
    y /= val.y;
    z /= val.z;
    w /= val.w;
    return *this;
}

/* basic_vec4::operator/= */
template <typename T>
template <typename S>
inline basic_vec4<T>& basic_vec4<T>::operator/=(S val) noexcept
{
    x /= val;
    y /= val;
    z /= val;
    w /= val;
    return *this;
}

/* basic_vec4::vec3 */
template <typename T>
inline const basic_vec3<T>& basic_vec4<T>::vec3() const noexcept
{
    return *(static_cast<basic_vec3<T>*>(static_cast<void*>(this)));
}

/* basic_vec4::vec3 */
template <typename T>
inline basic_vec3<T>& basic_vec4<T>::vec3() noexcept
{
    return *(static_cast<basic_vec3<T>*>(static_cast<void*>(this)));
}



/* operator+ */
template <typename T>
inline basic_vec4<T> operator+(const basic_vec4<T>& a, const basic_vec4<T>& b) noexcept
{
    return basic_vec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

/* operator- */
template <typename T>
inline basic_vec4<T> operator-(const basic_vec4<T>& a, const basic_vec4<T>& b) noexcept
{
    return basic_vec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

/* operator* */
template <typename T>
inline basic_vec4<T> operator*(const basic_vec4<T>& a, const basic_vec4<T>& b) noexcept
{
    return basic_vec4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

/* operator* */
template <typename T, typename S>
inline basic_vec4<T> operator*(const basic_vec4<T>& v, S s) noexcept
{
    return basic_vec4<T>(v.x * s, v.y * s, v.z * s, v.w * s);
}

/* operator* */
template <typename T, typename S>
inline basic_vec4<T> operator*(S s, const basic_vec4<T>& v) noexcept
{
    return basic_vec4<T>(v.x * s, v.y * s, v.z * s, v.w * s);
}

/* operator/ */
template <typename T>
inline basic_vec4<T> operator/(const basic_vec4<T>& a, const basic_vec4<T>& b) noexcept
{
    return basic_vec4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

/* operator/ */
template <typename T, typename S>
inline basic_vec4<T> operator/(const basic_vec4<T>& v, S s) noexcept
{
    return basic_vec4<T>(v.x / s, v.y / s, v.z / s, v.w / s);
}

/* operator/ */
template <typename T, typename S>
inline basic_vec4<T> operator/(S s, const basic_vec4<T>& v) noexcept
{
    return basic_vec4<T>(s / v.x, s / v.y, s / v.z, s / v.w);
}

using fvec4 = basic_vec4<float>;
using dvec4 = basic_vec4<double>;

} /* namespace green::core */
