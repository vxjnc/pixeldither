#include "error_diffusion.hpp"

#include <algorithm>

ErrorDiffusionDither::ErrorDiffusionDither(int transparentIndex) : m_transparentIndex(transparentIndex) {}

void ErrorDiffusionDither::start(const gd::Ref<gd::Image>& srcImage, double factor) {
    m_srcImage = srcImage;
    m_width = 2 + srcImage->get_width();
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

    const gd::Color& color = m_srcImage->get_pixel(x, y);

    int v[kChannels] = {color.get_r8(), color.get_g8(), color.get_b8(), color.get_a8()};
    for (int i = 0; i < kChannels; ++i) {
        v[i] += m_err[i][x + 1];
        v[i] = std::clamp(v[i], 0, 255);
    }

    const int index = palette.findBestfit(v[0], v[1], v[2], v[3], m_transparentIndex);

    const gd::Color& palColor = palette.getEntry(index);
    int palR = palColor.get_r8(), palG = palColor.get_g8(), palB = palColor.get_b8(),
        palA = palColor.get_a8();

    if (m_transparentIndex == index || palA == 0) {
        palR = color.get_r8();
        palG = color.get_g8();
        palB = color.get_b8();
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
