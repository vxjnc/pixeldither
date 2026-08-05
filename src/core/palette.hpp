#pragma once

#include "la8.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace Palette {
    inline constexpr int transparentIndex = 0;

    inline constexpr std::array<LA8, 3> colors = {
        LA8{0, 0},
        LA8{0, 255},
        LA8{255, 255},
    };

    [[nodiscard]] constexpr LA8 getEntry(std::size_t index) { return colors[index]; }

    [[nodiscard]] constexpr uint8_t findBestfit(LA8 color) {
        const uint8_t hasAlpha = static_cast<uint8_t>(color.a >= 8);
        const uint8_t isBright = static_cast<uint8_t>(color.l >= 128);
        return hasAlpha * (1 + isBright);
    }

    [[nodiscard]] constexpr int colorDistance(LA8 c1, LA8 c2) {
        int result = 0;
        if (c1.a && c2.a) {
            result += std::abs(static_cast<int>(c1.l) - static_cast<int>(c2.l)) * 2126;
        }
        result += std::abs(static_cast<int>(c1.a) - static_cast<int>(c2.a)) * 20000;
        return result;
    }
}
