#pragma once

#include "rgba8.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

class Palette {
public:
    constexpr LA8 getEntry(int index) const { return m_colors[index]; }

    constexpr uint32_t findBestfit(LA8 color) const { return m_lut[((color.l >> 3) << 5) | (color.a >> 3)]; }

    static constexpr int transparentIndex = 0;

private:
    static constexpr std::array<LA8, 3> m_colors = {
        LA8(0, 0),
        LA8(0, 255),
        LA8(255, 255),
    };

    static constexpr std::array<uint8_t, 32 * 32> m_lut = []() {
        std::array<uint8_t, 32 * 32> lut{};

        for (int l = 0; l < 32; ++l) {
            for (int a = 0; a < 32; ++a) {
                if (a == 0) {
                    lut[(l << 5) | a] = transparentIndex;
                    continue;
                }

                uint32_t bestfit = 1;
                uint32_t lowest = std::numeric_limits<uint32_t>::max();

                for (size_t i = 1; i < m_colors.size(); ++i) {
                    int dl = (m_colors[i].l >> 3) - l;
                    int da = (m_colors[i].a >> 3) - a;

                    uint32_t dist = (dl * dl * 100 * 100) + (da * da * 8 * 8);

                    if (dist < lowest) {
                        lowest = dist;
                        bestfit = static_cast<uint32_t>(i);
                    }
                }

                lut[(l << 5) | a] = static_cast<uint8_t>(bestfit);
            }
        }
        return lut;
    }();
};
