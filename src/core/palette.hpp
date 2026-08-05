#pragma once

#include "la8.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>

namespace Palette {
    inline constexpr int transparentIndex = 0;

    namespace detail {
        inline constexpr std::size_t lutSize = 32 * 32;

        inline constexpr std::array<LA8, 3> colors = {
            LA8{0, 0},
            LA8{0, 255},
            LA8{255, 255},
        };

        constexpr std::array<uint8_t, lutSize> generateLut() {
            std::array<uint8_t, lutSize> lut{};

            for (int l = 0; l < 32; ++l) {
                for (int a = 0; a < 32; ++a) {
                    if (a == 0) {
                        lut[(l << 5) | a] = transparentIndex;
                        continue;
                    }

                    uint32_t bestfit = 1;
                    uint32_t lowest = std::numeric_limits<uint32_t>::max();

                    for (std::size_t i = 1; i < colors.size(); ++i) {
                        int dl = (colors[i].l >> 3) - l;
                        int da = (colors[i].a >> 3) - a;

                        uint32_t dist = static_cast<uint32_t>((dl * dl * 100 * 100) + (da * da * 8 * 8));

                        if (dist < lowest) {
                            lowest = dist;
                            bestfit = static_cast<uint32_t>(i);
                        }
                    }

                    lut[(l << 5) | a] = static_cast<uint8_t>(bestfit);
                }
            }
            return lut;
        }

        inline constexpr auto lut = generateLut();

        [[nodiscard]] inline constexpr int const_abs(int x) { return x < 0 ? -x : x; }
    }

    [[nodiscard]] constexpr LA8 getEntry(std::size_t index) { return detail::colors[index]; }

    [[nodiscard]] constexpr uint8_t findBestfit(LA8 color) {
        const std::size_t lIndex = static_cast<std::size_t>(color.l >> 3);
        const std::size_t aIndex = static_cast<std::size_t>(color.a >> 3);
        return detail::lut[(lIndex << 5) | aIndex];
    }

    [[nodiscard]] constexpr int colorDistance(LA8 c1, LA8 c2) {
        int result = 0;
        if (c1.a && c2.a) {
            result += detail::const_abs(static_cast<int>(c1.l) - static_cast<int>(c2.l)) * 2126;
        }
        result += detail::const_abs(static_cast<int>(c1.a) - static_cast<int>(c2.a)) * 20000;
        return result;
    }
}
