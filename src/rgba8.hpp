#pragma once

#include <cstdint>

struct RGBA8 {
    constexpr RGBA8(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
    uint8_t r, g, b, a;
};
