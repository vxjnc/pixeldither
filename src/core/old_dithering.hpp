#pragma once

#include <algorithm>
#include <cstdint>

#include "core/la8.hpp"
#include "core/palette.hpp"

class OldDithering {
public:
    static constexpr int kMatrixSize = 8;

    static constexpr uint8_t kBayerMatrix8x8[kMatrixSize][kMatrixSize] = {
        {0, 32, 8, 40, 2, 34, 10, 42},    {48, 16, 56, 24, 50, 18, 58, 26}, {12, 44, 4, 36, 14, 46, 6, 38},
        {60, 28, 52, 20, 62, 30, 54, 22}, {3, 35, 11, 43, 1, 33, 9, 41},    {51, 19, 59, 27, 49, 17, 57, 25},
        {15, 47, 7, 39, 13, 45, 5, 37},   {63, 31, 55, 23, 61, 29, 53, 21}};

    static constexpr int kMaxMatrixValue = 63;

    LA8 ditherPixelToIndex(int x, int y, const LA8& srcPixel) {
        if (srcPixel.a == 0) {
            return Palette::getEntry(Palette::transparentIndex);
        }

        const uint8_t nearest1idx = Palette::findBestfit(srcPixel);
        const LA8 c1 = Palette::getEntry(nearest1idx);

        const int l2 = std::clamp(srcPixel.l - (c1.l - srcPixel.l), 0, 255);
        const int a2 = std::clamp(srcPixel.a - (c1.a - srcPixel.a), 0, 255);
        const uint8_t nearest2idx =
            Palette::findBestfit(LA8{static_cast<uint8_t>(l2), static_cast<uint8_t>(a2)});

        if (nearest1idx == nearest2idx) {
            return c1;
        }

        const LA8 c2 = Palette::getEntry(nearest2idx);

        const int d = Palette::colorDistance(c1, srcPixel);
        const int D = Palette::colorDistance(c1, c2);
        if (D == 0) {
            return c1;
        }

        const int scaled_d = kMaxMatrixValue * d / D;
        const int threshold = kBayerMatrix8x8[y % kMatrixSize][x % kMatrixSize];

        return (scaled_d > threshold) ? c2 : c1;
    }

    void dither_image_to_indexed(double factor, const LA8* srcData, int width, int height, LA8* dstData) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                dstData[y * width + x] = ditherPixelToIndex(x, y, srcData[y * width + x]);
            }
        }
    }
};
