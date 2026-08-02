#include "palette.hpp"

#include <limits>

void Palette::initBestfit() {
    for (int i = 1; i < 64; ++i) {
        int k = i * i;
        col_diff_l[i] = col_diff_l[128 - i] = k * 100 * 100;
        col_diff_a[i] = col_diff_a[128 - i] = k * 8 * 8;
    }
}

uint32_t Palette::findBestfit(LA8 color) const {
    color.l >>= 3;
    color.a >>= 3;

    if (color.a == 0 && transparentIndex >= 0) {
        return transparentIndex;
    }

    uint32_t bestfit = 0;
    uint32_t lowest = std::numeric_limits<uint32_t>::max();

    for (uint32_t i = 0; i < m_colors.size(); ++i) {
        if (i == transparentIndex) {
            continue;
        }

        LA8 la = m_colors[i];

        int dl = ((la.l >> 3) - color.l) & 127;
        int da = ((la.a >> 3) - color.a) & 127;

        uint32_t coldiff = col_diff_l[dl] + col_diff_a[da];

        if (coldiff < lowest) {
            if (coldiff == 0) {
                return i;
            }
            bestfit = i;
            lowest = coldiff;
        }
    }

    return bestfit;
}
