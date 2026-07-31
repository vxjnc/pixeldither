#pragma once

#include <cstdint>
#include <vector>

#include <godot_cpp/classes/color_palette.hpp>

namespace gd = godot;

class Palette {
public:
    void addEntry(const gd::Color& color);
    int size() const;
    gd::Color getEntry(int index) const;

    void initBestfit();
    int findBestfit(int r, int g, int b, int a, int mask_index) const;

private:
    std::vector<gd::Color> m_colors;

    std::vector<uint32_t> col_diff;
    uint32_t* col_diff_g = nullptr;
    uint32_t* col_diff_r = nullptr;
    uint32_t* col_diff_b = nullptr;
    uint32_t* col_diff_a = nullptr;
};
