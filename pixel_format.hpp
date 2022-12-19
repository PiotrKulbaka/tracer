#pragma once

#include <cstdint>

enum pixel_format : int32_t
{
    /* do not modify */
    red,    /* 1 channel */
    rg,     /* 2 channels */
    rgb,    /* 3 channels */
    bgr,    /* 3 channels */
    rgba,   /* 4 channels */
    bgra    /* 4 channels */
};

inline int32_t pixel_format_get_red_component_index(pixel_format format)
{
    constexpr int32_t red_component_index[] {0, 0, 0, 2, 0, 2};
    return red_component_index[format];
}

inline int32_t pixel_format_get_green_component_index(pixel_format format)
{
    constexpr int32_t green_component_index[] {-1, 1, 1, 1, 1, 1};
    return green_component_index[format];
}

inline int32_t pixel_format_get_blue_component_index(pixel_format format)
{
    constexpr int32_t blue_component_index[] {-1, -1, 2, 0, 2, 0};
    return blue_component_index[format];
}

inline int32_t pixel_format_get_alpha_component_index(pixel_format format)
{
    constexpr int32_t alpha_component_index[] {-1, -1, -1, -1, 3, 3};
    return alpha_component_index[format];
}

inline int32_t pixel_format_get_components_number(pixel_format format)
{
    constexpr int32_t components_number[] {1, 2, 3, 3, 4, 4};
    return components_number[format];
}
