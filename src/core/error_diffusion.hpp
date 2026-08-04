#pragma once

#include "palette.hpp"

#include <algorithm>
#include <ranges>

#include "core/error_buffer.hpp"

#if defined(__GNUC__) || defined(__clang__)
#define RESTRICT __restrict__
#elif defined(_MSC_VER)
#define RESTRICT __restrict
#else
#define RESTRICT
#endif

class ErrorDiffusionDither {
public:
    static constexpr int kChannels = sizeof(LA8);

    void start(const LA8* RESTRICT srcData, int width, int height, double factor) {
        m_srcData = srcData;
        m_srcWidth = width;
        m_width = 2 + width;
        m_err.resize(m_width);
        m_factor = int(factor * 100.0);
    }

    void beginRow(int y) { m_err.swapAndResetNext(); }

    int ditherRgbToIndex2D(int x, int y, const Palette& palette) {
        const LA8& srcPixel = m_srcData[y * m_srcWidth + x];

        LA8 v = srcPixel;
        v.l = std::clamp(v.l + m_err.curr[0][x + 1], 0, 255);
        v.a = std::clamp(v.a + m_err.curr[1][x + 1], 0, 255);

        const int index = palette.findBestfit(v);

        LA8 palColor = palette.getEntry(index);

        if (palette.transparentIndex == index || palColor.a == 0) {
            palColor.l = srcPixel.l;
            palColor.a = 0;
        }

        const int quantError[kChannels] = {v.l - palColor.l, v.a - palColor.a};

        for (int i = 0; i < kChannels; ++i) {
            const int q = quantError[i] * m_factor / 100;
            const int a = q * 7 / 16;
            const int b = q * 3 / 16;
            const int c = q * 5 / 16;
            const int d = q * 1 / 16;

            if (y & 1) {
                m_err.curr[i][x] += a;
                m_err.next[i][x + 2] += b;
                m_err.next[i][x + 1] += c;
                m_err.next[i][x] += d;
            }
            else {
                m_err.curr[i][x + 2] += a;
                m_err.next[i][x] += b;
                m_err.next[i][x + 1] += c;
                m_err.next[i][x + 2] += d;
            }
        }

        return index;
    }

    void dither_image_to_indexed(double factor, const LA8* RESTRICT srcData, int width, int height,
                                 LA8* RESTRICT dstData, const Palette& palette) {
        start(srcData, width, height, factor);

        for (int y = 0; y < height; ++y) {
            beginRow(y);

            auto process = [&](auto row) {
                for (int x : row) {
                    dstData[y * width + x] = palette.getEntry(ditherRgbToIndex2D(x, y, palette));
                }
            };

            if (y & 1) {
                process(std::views::iota(0, width) | std::views::reverse);
            }
            else {
                process(std::views::iota(0, width));
            }
        }
    }

private:
    const LA8* RESTRICT m_srcData = nullptr;
    int m_srcWidth = 0;

    int m_width = 0;
    ErrorBuffer<kChannels> m_err;
    int m_factor = 0;
};
