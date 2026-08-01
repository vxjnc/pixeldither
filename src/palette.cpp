#include "palette.hpp"

#include <limits>

void Palette::initBestfit() {
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

        int dr = ((rgba.r >> 3) - r) & 127;
        int dg = ((rgba.g >> 3) - g) & 127;
        int db = ((rgba.b >> 3) - b) & 127;
        int da = ((rgba.a >> 3) - a) & 127;

        int coldiff = col_diff_r[dr] + col_diff_g[dg] + col_diff_b[db] + col_diff_a[da];

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
