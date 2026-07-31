#include "error_diffusion.hpp"

#include <algorithm>

void ErrorDiffusionDither::start(const uint8_t* srcData, int width, int height, double factor) {
    m_srcData = srcData;
    m_srcWidth = width;
    m_width = 2 + width;
    for (int i = 0; i < kChannels; ++i) {
        m_err[i].resize(m_width * 2, 0);
    }
    m_lastY = -1;
    m_factor = int(factor * 100.0);
}

int ErrorDiffusionDither::ditherRgbToIndex2D(int x, int y, const Palette& palette) {
    if (y != m_lastY) {
        for (int i = 0; i < kChannels; ++i) {
            int* row0 = &m_err[i][0];
            int* row1 = row0 + m_width;
            int* end1 = row1 + m_width;
            std::copy(row1, end1, row0);
            std::fill(row1, end1, 0);
        }
        m_lastY = y;
    }

    const uint8_t* srcPixel = &m_srcData[(y * m_srcWidth + x) * 4];
    int origR = srcPixel[0], origG = srcPixel[1], origB = srcPixel[2], origA = srcPixel[3];

    int v[kChannels] = {origR, origG, origB, origA};
    for (int i = 0; i < kChannels; ++i) {
        v[i] += m_err[i][x + 1];
        v[i] = std::clamp(v[i], 0, 255);
    }

    const int index = palette.findBestfit(v[0], v[1], v[2], v[3]);

    RGBA8 palColor = palette.getEntry(index);
    int palR = palColor.r, palG = palColor.g, palB = palColor.b, palA = palColor.a;

    if (palette.transparentIndex == index || palA == 0) {
        palR = origR;
        palG = origG;
        palB = origB;
        palA = 0;
    }

    const int quantError[kChannels] = {v[0] - palR, v[1] - palG, v[2] - palB, v[3] - palA};

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
            err[+2] += a;
            err[m_width] += b;
            err[m_width + 1] += c;
            err[m_width + 2] += d;
        }
    }

    return index;
}
