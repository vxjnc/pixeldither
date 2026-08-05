#pragma once

#include <algorithm>

#include "core/error_buffer.hpp"
#include "core/la8.hpp"
#include "core/palette.hpp"
#include "core/restrict.hpp"

class ErrorDiffusionDither {
public:
    enum class ScanDirection : uint8_t { Forward, Reverse };

    static constexpr int kChannels = sizeof(LA8);

    void start(const LA8* RESTRICT srcData, int width, int height, double factor) {
        m_srcData = srcData;
        m_srcWidth = width;
        m_width = 2 + width;
        m_err.resize(m_width);
        m_factor = int(factor * 100.0);
    }

    void beginRow(int y) {
        m_err.swapAndResetNext();
        for (int i = 0; i < kChannels; ++i) {
            m_carry[i] = 0;
            m_delayNear[i] = 0;
            m_delayFar[i] = 0;
        }
    }

    template <ScanDirection Dir> LA8 ditherRgbToIndex2D(int x, int y) {
        const LA8 srcPixel = m_srcData[y * m_srcWidth + x];

        const int v_l = std::clamp(srcPixel.l + m_err.curr[0][x + 1] + m_carry[0], 0, 255);
        const int v_a = std::clamp(srcPixel.a + m_err.curr[1][x + 1] + m_carry[1], 0, 255);

        const int index = Palette::findBestfit(LA8{static_cast<uint8_t>(v_l), static_cast<uint8_t>(v_a)});

        const LA8 pal = Palette::getEntry(index);

        const int quantError[kChannels] = {v_l - pal.l, v_a - pal.a};

        for (int i = 0; i < kChannels; ++i) {
            const int q = (quantError[i] * m_factor) / 100;

            const int a = (q * 7) >> 4;
            const int b = (q * 3) >> 4;
            const int c = (q * 5) >> 4;
            const int d = q >> 4;

            int* RESTRICT next_row = m_err.next[i].data();

            if constexpr (Dir == ScanDirection::Reverse) {
                next_row[x + 2] += m_delayFar[i] + b;
            }
            else {
                next_row[x] += m_delayFar[i] + b;
            }
            m_delayFar[i] = m_delayNear[i] + c;
            m_delayNear[i] = d;

            m_carry[i] = a;
        }

        return Palette::getEntry(index);
    }

    template <ScanDirection Dir> void flushRowTail(int width) {
        for (int i = 0; i < kChannels; ++i) {
            int* RESTRICT next_row = m_err.next[i].data();

            if constexpr (Dir == ScanDirection::Reverse) {
                next_row[1] += m_delayFar[i];
                next_row[0] += m_delayNear[i];
            }
            else {
                next_row[width] += m_delayFar[i];
                next_row[width + 1] += m_delayNear[i];
            }
        }
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
            flushRowTail<ScanDirection::Forward>(width);

            // --- 2. Odd row (y + 1) -> Reverse scan ---
            ++y;
            beginRow(y);
            for (int x = width - 1; x >= 0; --x) {
                dstData[y * width + x] = ditherRgbToIndex2D<ScanDirection::Reverse>(x, y);
            }
            flushRowTail<ScanDirection::Reverse>(width);
        }

        // Tail: handle the remaining (even) row if height is odd
        if (y < height) {
            beginRow(y);
            for (int x = 0; x < width; ++x) {
                dstData[y * width + x] = ditherRgbToIndex2D<ScanDirection::Forward>(x, y);
            }
            flushRowTail<ScanDirection::Forward>(width);
        }
    }

private:
    const LA8* RESTRICT m_srcData = nullptr;
    int m_srcWidth = 0;

    int m_width = 0;
    ErrorBuffer<kChannels> m_err;
    int m_factor = 0;

    int m_carry[kChannels] = {0, 0};
    int m_delayNear[kChannels] = {0, 0};
    int m_delayFar[kChannels] = {0, 0};
};
