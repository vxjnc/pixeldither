#pragma once

#include "palette.hpp"

#include <cstdint>
#include <vector>

class ErrorDiffusionDither {
public:
    static constexpr int kChannels = 4;

    void start(const uint8_t* rgbaData, int width, int height, double factor);
    void beginRow(int y);
    int ditherRgbToIndex2D(int x, int y, const Palette& palette);

private:
    const uint8_t* m_srcData = nullptr;
    int m_srcWidth = 0;

    int m_width = 0;
    std::vector<int> m_err[kChannels];
    int m_lastY = -1;
    int m_factor = 0;
};
