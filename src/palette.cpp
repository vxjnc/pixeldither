#include "palette.hpp"

#include <limits>

void Palette::initBestfit() {
    col_diff_r = &col_diff[128 * 0];
    col_diff_g = &col_diff[128 * 1];
    col_diff_b = &col_diff[128 * 2];
    col_diff_a = &col_diff[128 * 3];

    for (int i = 1; i < 64; ++i) {
        int k = i * i;
        col_diff_r[i] = col_diff_r[128 - i] = k * 30 * 30;
        col_diff_g[i] = col_diff_g[128 - i] = k * 59 * 59;
        col_diff_b[i] = col_diff_b[128 - i] = k * 11 * 11;
        col_diff_a[i] = col_diff_a[128 - i] = k * 8 * 8;
    }
}

int Palette::findBestfit(int r, int g, int b, int a) const {
    r >>= 3;
    g >>= 3;
    b >>= 3;
    a >>= 3;

    if (a == 0 && transparentIndex >= 0) {
        return transparentIndex;
    }

    int bestfit = 0;
    int lowest = std::numeric_limits<int>::max();

    for (int i = 0; i < m_colors.size(); ++i) {
        if (i == transparentIndex) {
            continue;
        }

        RGBA8 rgba = m_colors[i];

        uint32_t cr = rgba.r;
        uint32_t cg = rgba.g;
        uint32_t cb = rgba.b;
        uint32_t ca = rgba.a;

        int coldiff = col_diff_g[((cg >> 3) - g) & 127];
        if (coldiff < lowest) {
            coldiff += col_diff_r[((cr >> 3) - r) & 127];
            if (coldiff < lowest) {
                coldiff += col_diff_b[((cb >> 3) - b) & 127];
                if (coldiff < lowest) {
                    coldiff += col_diff_a[((ca >> 3) - a) & 127];
                    if (coldiff < lowest) {
                        if (coldiff == 0) {
                            return i;
                        }
                        bestfit = i;
                        lowest = coldiff;
                    }
                }
            }
        }
    }

    return bestfit;
}
