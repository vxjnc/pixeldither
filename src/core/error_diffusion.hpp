#pragma once

#include <algorithm>

#include "core/error_buffer.hpp"
#include "core/la8.hpp"

#if defined(__GNUC__) || defined(__clang__)
#define RESTRICT __restrict__
#elif defined(_MSC_VER)
#define RESTRICT __restrict
#else
#define RESTRICT
#endif

class ErrorDiffusionDither {
public:
    enum class ScanDirection : uint8_t { Forward, Reverse };

    static constexpr int kChannels = sizeof(LA8);

    constexpr LA8 getPaletteColor(int index) { return LA8(index == 2 ? 255 : 0, index > 0 ? 255 : 0); }

    void start(const LA8* RESTRICT srcData, int width, int height, double factor) {
        m_srcData = srcData;
        m_srcWidth = width;
        m_width = 2 + width;
        m_err.resize(m_width);
        m_factor = int(factor * 100.0);
    }

    void beginRow(int y) {
        m_err.swapAndResetNext();
        m_carry[0] = 0;
        m_carry[1] = 0;
    }

    template <ScanDirection Dir> LA8 ditherRgbToIndex2D(int x, int y) {
        const LA8 srcPixel = m_srcData[y * m_srcWidth + x];

        const int v_l = std::clamp(srcPixel.l + m_err.curr[0][x + 1] + m_carry[0], 0, 255);
        const int v_a = std::clamp(srcPixel.a + m_err.curr[1][x + 1] + m_carry[1], 0, 255);

        const bool hasAlpha = (v_a >= 128);
        const bool isWhite = (v_l >= 128);

        const int index = hasAlpha ? (isWhite ? 2 : 1) : 0;

        const int pal_l = hasAlpha ? (isWhite ? 255 : 0) : static_cast<int>(srcPixel.l);
        const int pal_a = hasAlpha ? 255 : 0;

        const int quantError[kChannels] = {v_l - pal_l, v_a - pal_a};

        for (int i = 0; i < kChannels; ++i) {
            const int q = (quantError[i] * m_factor) / 100;

            const int corr = (q >> 31) & 15;
            const int a = ((q * 7) + corr) >> 4;
            const int b = ((q * 3) + corr) >> 4;
            const int c = ((q * 5) + corr) >> 4;
            const int d = (q + corr) >> 4;

            int* RESTRICT next_row = m_err.next[i].data();

            if constexpr (Dir == ScanDirection::Reverse) {
                next_row[x + 2] += b;
                next_row[x + 1] += c;
                next_row[x] += d;
            }
            else {
                next_row[x] += b;
                next_row[x + 1] += c;
                next_row[x + 2] += d;
            }

            m_carry[i] = a;
        }

        return getPaletteColor(index);
    }

    void dither_image_to_indexed(double factor, const LA8* RESTRICT srcData, int width, int height,
                                 LA8* RESTRICT dstData) {
        start(srcData, width, height, factor);

        int y = 0;
        for (; y + 1 < height; ++y) {
            // --- 1. Even row (y) -> Forward scan ---
            beginRow(y);
            for (int x = 0; x < width; ++x) {
                dstData[y * width + x] = ditherRgbToIndex2D<ScanDirection::Forward>(x, y);
            }

            // --- 2. Odd row (y + 1) -> Reverse scan ---
            ++y;
            beginRow(y);
            for (int x = width - 1; x >= 0; --x) {
                dstData[y * width + x] = ditherRgbToIndex2D<ScanDirection::Reverse>(x, y);
            }
        }

        // Tail: handle the remaining (even) row if height is odd
        if (y < height) {
            beginRow(y);
            for (int x = 0; x < width; ++x) {
                dstData[y * width + x] = ditherRgbToIndex2D<ScanDirection::Forward>(x, y);
            }
        }
    }

private:
    const LA8* RESTRICT m_srcData = nullptr;
    int m_srcWidth = 0;

    int m_width = 0;
    ErrorBuffer<kChannels> m_err;
    int m_factor = 0;

    int m_carry[kChannels] = {0, 0};
};
