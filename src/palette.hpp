#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include <godot_cpp/variant/color.hpp>

namespace gd = godot;

class Palette {
public:
    explicit Palette(std::span<const gd::Color> colors);

    const gd::Color& getEntry(int index) const;
    int findBestfit(int r, int g, int b, int a, int mask_index) const;

private:
    void initBestfit();

    std::vector<gd::Color> m_colors;

    std::array<uint32_t, 4 * 128> col_diff{};
    uint32_t* col_diff_g = nullptr;
    uint32_t* col_diff_r = nullptr;
    uint32_t* col_diff_b = nullptr;
    uint32_t* col_diff_a = nullptr;
};
