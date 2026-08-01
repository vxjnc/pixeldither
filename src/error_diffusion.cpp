#include "error_diffusion.hpp"

#include <algorithm>

void ErrorDiffusionDither::start(const LA8* srcData, int width, int height, double factor) {
    m_srcData = srcData;
    m_srcWidth = width;
    m_width = 2 + width;
    for (int i = 0; i < kChannels; ++i) {
        m_err[i].resize(m_width * 2, 0);
    }
    m_factor = int(factor * 100.0);
}

void ErrorDiffusionDither::beginRow(int y) {
    for (int i = 0; i < kChannels; ++i) {
        int* row0 = &m_err[i][0];
        int* row1 = row0 + m_width;
        int* end1 = row1 + m_width;
        std::copy(row1, end1, row0);
        std::fill(row1, end1, 0);
    }
}

int ErrorDiffusionDither::ditherRgbToIndex2D(int x, int y, const Palette& palette) {
    const LA8& srcPixel = m_srcData[y * m_srcWidth + x];

    LA8 v = srcPixel;
    v.l = std::clamp(v.l + m_err[0][x + 1], 0, 255);
    v.a = std::clamp(v.a + m_err[1][x + 1], 0, 255);

    const int index = palette.findBestfit(v);

    LA8 palColor = palette.getEntry(index);

    if (palette.transparentIndex == index || palColor.a == 0) {
        palColor.l = srcPixel.l;
        palColor.a = 0;
    }

    const int quantError[kChannels] = {v.l - palColor.l, v.a - palColor.a};

    for (int i = 0; i < kChannels; ++i) {
        int* err = &m_err[i][x];
        const int q = quantError[i] * m_factor / 100;
        const int a = q * 7 / 16;
        const int b = q * 3 / 16;
        const int c = q * 5 / 16;
        const int d = q * 1 / 16;

        if (y & 1) {
            err[0] += a;
            err[m_width + 2] += b;
            err[m_width + 1] += c;
            err[m_width] += d;
        }
        else {
            err[2] += a;
            err[m_width] += b;
            err[m_width + 1] += c;
            err[m_width + 2] += d;
        }
    }

    return index;
}
