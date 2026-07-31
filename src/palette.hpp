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

    std::array<uint32_t, 4 * 128> col_diff{};
    uint32_t* col_diff_r = nullptr;
    uint32_t* col_diff_g = nullptr;
    uint32_t* col_diff_b = nullptr;
    uint32_t* col_diff_a = nullptr;
};
