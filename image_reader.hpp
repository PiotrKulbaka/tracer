#pragma once

#include <string>

class image
{
public:
    template <class T>
    static bool write(const T& image, const std::string& path);
};
