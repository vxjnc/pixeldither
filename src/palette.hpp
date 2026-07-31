#pragma once

#include <array>
#include <cstdint>

#include <godot_cpp/variant/color.hpp>

namespace gd = godot;

class Palette {
public:
    Palette();

    const gd::Color& getEntry(int index) const { return m_colors[index]; }
    int findBestfit(int r, int g, int b, int a) const;

    static constexpr int transparentIndex = 0;

private:
    void initBestfit();

    const std::array<gd::Color, 3> m_colors = {
        gd::Color(0, 0, 0, 0),
        gd::Color(0, 0, 0, 1),
        gd::Color(1, 1, 1, 1),
    };

    std::array<uint32_t, 4 * 128> col_diff{};
    uint32_t* col_diff_r = nullptr;
    uint32_t* col_diff_g = nullptr;
    uint32_t* col_diff_b = nullptr;
    uint32_t* col_diff_a = nullptr;
};
