#pragma once

#include "rgba8.hpp"

#include <array>
#include <cstdint>

class Palette {
public:
    Palette() { initBestfit(); }

    constexpr LA8 getEntry(int index) const { return m_colors[index]; }
    uint32_t findBestfit(LA8 color) const;

    static constexpr int transparentIndex = 0;

private:
    void initBestfit();

    static constexpr std::array<LA8, 3> m_colors = {
        LA8(0, 0),
        LA8(0, 255),
        LA8(255, 255),
    };

    std::array<uint32_t, 128> col_diff_l{};
    std::array<uint32_t, 128> col_diff_a{};
};
