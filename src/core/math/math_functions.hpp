#pragma once

#include <cmath>

namespace green::core::math
{

constexpr double pi = 3.14159265358979323846;

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

template <typename T> T     min(T a, T b) noexcept;
template <typename T> T     max(T a, T b) noexcept;
template <typename T> T     sign(T a) noexcept;
template <typename T> T     sqr(T a) noexcept;
template <typename T> T     abs(T a) noexcept;
template <typename T> T     sqrt(T a) noexcept;
template <typename T> T     sin(T a) noexcept;
template <typename T> T     cos(T a) noexcept;
template <typename T> T     tan(T a) noexcept;
template <typename T> T     asin(T a) noexcept;
template <typename T> T     acos(T a) noexcept;
template <typename T> T     atan(T a) noexcept;
template <typename T> T     atan2(T x, T y) noexcept;
template <typename T> T     log(T a) noexcept;
template <typename T> T     exp(T a) noexcept;
template <typename Ta, typename Tb, typename Tr>
Tr                          pow(Ta base, Tb power) noexcept;
template <typename T> T     deg2rad(T a) noexcept;
template <typename T> T     rad2deg(T a) noexcept;
template <typename T> T     floor(T a) noexcept;
template <typename T> T     ceil(T a) noexcept;
template <typename T> T     frac(T a) noexcept;
template <typename T> T     round(T a) noexcept;
template <typename T> T     clamp(T val, T min, T max);



template <typename T>
T min(T a, T b) noexcept
{
    return a < b ? a : b;
}

template <typename T>
T max(T a, T b) noexcept
{
    return a > b ? a : b;
}

template <typename T>
T sign(T a) noexcept
{
    constexpr T zero = static_cast<T>(0);
    return a > zero ? static_cast<T>(1) : (a < zero ? -static_cast<T>(1) : zero);
}

template <typename T>
T sqr(T a) noexcept
{
    return a * a;
}

template <typename T>
T abs(T a) noexcept
{
    return std::abs(a);
}

template <typename T>
T sqrt(T a) noexcept
{
    return std::sqrt(a);
}

template <typename T>
T sin(T a) noexcept
{
    return std::sin(a);
}

template <typename T>
T cos(T a) noexcept
{
    return std::cos(a);
}

template <typename T>
T tan(T a) noexcept
{
    return std::tan(a);
}

template <typename T>
T asin(T a) noexcept
{
    return std::asin(a);
}

template <typename T>
T acos(T a) noexcept
{
    return std::acos(a);
}

template <typename T>
T atan(T a) noexcept
{
    return std::atan(a);
}

template <typename T>
T atan2(T x, T y) noexcept
{
    return std::atan2(x, y);
}

template <typename T>
T log(T a) noexcept
{
    return std::log(a);
}

template <typename T>
T exp(T a) noexcept
{
    return std::exp(a);
}

template <typename Ta, typename Tb, typename Tr>
Tr pow(Ta base, Tb power) noexcept
{
    return std::pow(base, power);
}

template <typename T>
T deg2rad(T a) noexcept
{
    return a * pi / 180.0;
}

template <typename T>
T rad2deg(T a) noexcept
{
    return a * 180.0 / pi;
}

template <typename T>
T floor(T a) noexcept
{
    return std::floor(a);
}

template <typename T>
T ceil(T a) noexcept
{
    return std::ceil(a);
}

template <typename T>
T frac(T a) noexcept
{
    return a - floor(a);
}

template <typename T>
T round(T a) noexcept
{
    return floor(a + static_cast<T>(0.5));
}

template <typename T>
T clamp(T val, T min, T max)
{
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

} /* namespace green::core::math */
