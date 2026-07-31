#pragma once

#include "palette.hpp"

#include <cstring>
#include <vector>

#include <godot_cpp/classes/image.hpp>

class ErrorDiffusionDither {
public:
    static constexpr int kChannels = 4;

    void start(const gd::Ref<gd::Image>& srcImage, double factor);
    int ditherRgbToIndex2D(int x, int y, const Palette& palette);

private:
    gd::Ref<gd::Image> m_srcImage;

    int m_width = 0;
    std::vector<int> m_err[kChannels];
    int m_lastY = -1;
    int m_factor = 0;
};
