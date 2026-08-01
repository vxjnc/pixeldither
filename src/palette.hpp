#pragma once

#include "rgba8.hpp"

#include <array>
#include <cstdint>

class Palette {
public:
    Palette() { initBestfit(); }

    RGBA8 getEntry(int index) const { return m_colors[index]; }
    int findBestfit(int r, int g, int b, int a) const;

    static constexpr int transparentIndex = 0;

private:
    void initBestfit();

    static constexpr std::array<RGBA8, 3> m_colors = {
        RGBA8(0, 0, 0, 0),
        RGBA8(0, 0, 0, 255),
        RGBA8(255, 255, 255, 255),
    };

    std::array<uint32_t, 128> col_diff_r{};
    std::array<uint32_t, 128> col_diff_g{};
    std::array<uint32_t, 128> col_diff_b{};
    std::array<uint32_t, 128> col_diff_a{};
};
