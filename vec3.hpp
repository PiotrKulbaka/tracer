#pragma once

#include <cmath>

template <typename T>
class basic_vec3
{
public:
    using value_type = T;

public:
                    basic_vec3() = default;
                    basic_vec3(T x) noexcept;
                    basic_vec3(T x, T y, T z) noexcept;

    basic_vec3&     operator+=(const basic_vec3& val) noexcept;
    basic_vec3&     operator-=(const basic_vec3& val) noexcept;
    template <typename S>
    basic_vec3&     operator*=(S val) noexcept;
    basic_vec3&     operator*=(const basic_vec3& val) noexcept;
    template <typename S>
    basic_vec3&     operator/=(S val) noexcept;

    basic_vec3      cross(const basic_vec3& val) const noexcept;
    T               dot(const basic_vec3& val) const noexcept;

    basic_vec3      normalize() const noexcept;
    template <typename S>
    basic_vec3      normalize(S& len) const noexcept;
    basic_vec3&     normalize_self() noexcept;
    template <typename S>
    basic_vec3&     normalize_self(S& len) noexcept;

    basic_vec3      abs() const noexcept;
    basic_vec3&     abs_self() noexcept;

    T               min() const noexcept;
    T               max() const noexcept;

    basic_vec3      sign() const noexcept;
public:
    T x, y, z;
};



template <typename T>
inline basic_vec3<T>::basic_vec3(T x) noexcept
    : x(x)
    , y(x)
    , z(x)
{
}

template <typename T>
inline basic_vec3<T>::basic_vec3(T x, T y, T z) noexcept
    : x(x)
    , y(y)
    , z(z)
{
}

template <typename T>
inline basic_vec3<T>& basic_vec3<T>::operator+=(const basic_vec3<T>& val) noexcept
{
    x += val.x;
    y += val.y;
    z += val.z;
    return *this;
}

template <typename T>
inline basic_vec3<T>& basic_vec3<T>::operator-=(const basic_vec3<T>& val) noexcept
{
    x -= val.x;
    y -= val.y;
    z -= val.z;
    return *this;
}

template <typename T>
template <typename S>
inline basic_vec3<T>& basic_vec3<T>::operator*=(S val) noexcept
{
    x *= val;
    y *= val;
    z *= val;
    return *this;
}

template <typename T>
inline basic_vec3<T>& basic_vec3<T>::operator*=(const basic_vec3<T>& val) noexcept
{
    x *= val.x;
    y *= val.y;
    z *= val.z;
    return *this;
}

template <typename T>
template <typename S>
inline basic_vec3<T>& basic_vec3<T>::operator/=(S val) noexcept
{
    x /= val;
    y /= val;
    z /= val;
    return *this;
}

template <typename T>
inline basic_vec3<T> basic_vec3<T>::cross(const basic_vec3<T>& val) const noexcept
{
    return basic_vec3<T>(y * val.z - z * val.y, z * val.x - x * val.z, x * val.y - y * val.x);
}

template <typename T>
inline T basic_vec3<T>::dot(const basic_vec3<T>& val) const noexcept
{
    return x * val.x + y * val.y + z * val.z;
}

template <typename T>
inline basic_vec3<T> basic_vec3<T>::normalize() const noexcept
{
    auto len = std::sqrt(dot(*this));
    return basic_vec3<T>(x / len, y / len, z / len);
}

template <typename T>
template <typename S>
inline basic_vec3<T> basic_vec3<T>::normalize(S& len) const noexcept
{
    len = std::sqrt(dot(*this));
    return basic_vec3<T>(x / len, y / len, z / len);
}

template <typename T>
inline basic_vec3<T>& basic_vec3<T>::normalize_self() noexcept
{
    *this /= std::sqrt(dot(*this));
    return *this;
}

template <typename T>
template <typename S>
inline basic_vec3<T>& basic_vec3<T>::normalize_self(S& len) noexcept
{
    len = std::sqrt(dot(*this));
    *this /= len;
    return *this;
}

template <typename T>
inline basic_vec3<T> basic_vec3<T>::abs() const noexcept
{
    return basic_vec3<T>(std::abs(x), std::abs(y), std::abs(z));
}

template <typename T>
inline basic_vec3<T>& basic_vec3<T>::abs_self() noexcept
{
    x = std::abs(x);
    y = std::abs(y);
    z = std::abs(z);
    return *this;
}

template <typename T>
inline T basic_vec3<T>::min() const noexcept
{
    return x < y ? (x < z ? x : z) : (y < z ? y : z);
}

template <typename T>
inline T basic_vec3<T>::max() const noexcept
{
    return x > y ? (x > z ? x : z) : (y > z ? y : z);
}

template <typename T>
inline basic_vec3<T> basic_vec3<T>::sign() const noexcept
{
    return basic_vec3<T>(
        x < 0.0 ? -1.0 : (x > 0.0 ? 1.0 : 0.0),
        y < 0.0 ? -1.0 : (y > 0.0 ? 1.0 : 0.0),
        z < 0.0 ? -1.0 : (z > 0.0 ? 1.0 : 0.0));
}



template <typename T>
inline basic_vec3<T> operator-(const basic_vec3<T>& v) noexcept
{
    return basic_vec3<T>(-v.x, -v.y, -v.z);
}

template <typename T>
inline basic_vec3<T> operator+(const basic_vec3<T>& a, const basic_vec3<T>& b) noexcept
{
    return basic_vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
}

template <typename T>
inline basic_vec3<T> operator-(const basic_vec3<T>& a, const basic_vec3<T>& b) noexcept
{
    return basic_vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template <typename T, typename S>
inline basic_vec3<T> operator*(const basic_vec3<T>& v, S s) noexcept
{
    return basic_vec3<T>(v.x * s, v.y * s, v.z * s);
}

template <typename T>
inline basic_vec3<T> operator*(const basic_vec3<T>& a, const basic_vec3<T>& b) noexcept
{
    return basic_vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
}

template <typename T, typename S>
inline basic_vec3<T> operator*(S s, const basic_vec3<T>& v) noexcept
{
    return basic_vec3<T>(s * v.x, s * v.y, s * v.z);
}

template <typename T, typename S>
inline basic_vec3<T> operator/(const basic_vec3<T>& v, S s) noexcept
{
    return basic_vec3<T>(v.x / s, v.y / s, v.z / s);
}

template <typename T>
inline basic_vec3<T> operator/(const basic_vec3<T>& a, const basic_vec3<T>& b) noexcept
{
    return basic_vec3<T>(a.x / b.x, a.y / b.y, a.z / b.z);
}

template <typename T, typename S>
inline basic_vec3<T> operator/(S s, const basic_vec3<T>& v) noexcept
{
    return basic_vec3<T>(s / v.x, s / v.y, s / v.z);
}
