#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>

#include "core/la8.hpp"
#include "core/palette.hpp"

class OrderedDithering {
public:
    static constexpr int kMatrixSize = 4;

    static constexpr uint8_t kBayerMatrix4x4[kMatrixSize][kMatrixSize] = {
        {0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};

    static constexpr int kMaxMatrixValue = 15;

    LA8 ditherPixelToIndex(int x, int y, const LA8& srcPixel) {
        if (srcPixel.a == 0) {
            return Palette::getEntry(Palette::transparentIndex);
        }

        const uint8_t index = Palette::findBestfit(srcPixel);
        const LA8 c0 = Palette::getEntry(index);

        int bestMix = 0;
        int altIndex = -1;
        int closestDistance = std::numeric_limits<int>::max();

        for (uint8_t i = 0; i < Palette::detail::colors.size(); ++i) {
            if (i == Palette::transparentIndex) {
                continue;
            }

            const LA8 c1 = Palette::getEntry(i);

            int mix = 0;
            int div = 0;

            if (srcPixel.a && c0.a && c1.a) {
                if (c1.l - c0.l) {
                    mix += 2126 * kMaxMatrixValue * (srcPixel.l - c0.l) / (c1.l - c0.l);
                    div += 2126;
                }
            }
            if (c1.a - c0.a) {
                mix += 20000 * kMaxMatrixValue * (srcPixel.a - c0.a) / (c1.a - c0.a);
                div += 20000;
            }
            if (mix) {
                if (div) {
                    mix /= div;
                }
                mix = std::clamp(mix, 0, kMaxMatrixValue);
            }

            const int lM = c0.l + (c1.l - c0.l) * mix / kMaxMatrixValue;
            const int aM = c0.a + (c1.a - c0.a) * mix / kMaxMatrixValue;

            const int d =
                Palette::colorDistance(srcPixel, LA8{static_cast<uint8_t>(lM), static_cast<uint8_t>(aM)}) +
                Palette::colorDistance(c0, c1) / 10;

            if (closestDistance > d) {
                closestDistance = d;
                bestMix = mix;
                altIndex = i;
            }
        }

        const int threshold = kBayerMatrix4x4[y % kMatrixSize][x % kMatrixSize];

        if (altIndex >= 0 && threshold < bestMix) {
            return Palette::getEntry(altIndex);
        }
        return c0;
    }

    void dither_image_to_indexed(double factor, const LA8* srcData, int width, int height, LA8* dstData) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                dstData[y * width + x] = ditherPixelToIndex(x, y, srcData[y * width + x]);
            }
        }
    }
};
