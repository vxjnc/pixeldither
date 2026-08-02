#pragma once

#include "palette.hpp"

#include "core/error_buffer.hpp"

class ErrorDiffusionDither {
public:
    static constexpr int kChannels = sizeof(LA8);

    void start(const LA8* srcData, int width, int height, double factor);
    void beginRow(int y) { m_err.swapAndResetNext(); }
    int ditherRgbToIndex2D(int x, int y, const Palette& palette);

    void dither_image_to_indexed(double factor, const LA8* srcData, int width, int height, LA8* dstData,
                                 const Palette& palette);

private:
    const LA8* m_srcData = nullptr;
    int m_srcWidth = 0;

    int m_width = 0;
    ErrorBuffer<kChannels> m_err;
    int m_factor = 0;
};
