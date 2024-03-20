#ifndef UTIL_H
#define UTIL_H

#include "splashkit.h"
#include "stdio.h"

extern color space_colors[];

template<typename T>
T choose(const T array[], int size);

template<typename T>
T clamp(const T& value, const T& low, const T& high);

color color_from_hex(const std::string& hex);

color color_from_hex(const std::string& hex) {
    int r, g, b;
    sscanf(hex.c_str(), "#%02x%02x%02x", &r, &g, &b);
    return rgb_color(r, g, b);
}

color space_colors[] = {
        color_from_hex("#BD251F"),
        color_from_hex("#FF5227"),
        color_from_hex("#FF9F37"),
        color_from_hex("#243A4B"),
        color_from_hex("#4AC0AF"),
        color_from_hex("#ACE5BA"),
        color_from_hex("#FFF1C1")
};

template<typename T>
T choose(const T array[], int size) {
    int index = std::rand() % size;
    return array[index];
}

template<typename T>
T clamp(const T& value, const T& low, const T& high) {
    return (value < low) ? low : (value > high) ? high : value;
}

#endif // UTIL_H
