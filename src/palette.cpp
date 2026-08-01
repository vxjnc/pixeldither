#include "palette.hpp"

#include <limits>

void Palette::initBestfit() {
    for (int i = 1; i < 64; ++i) {
        int k = i * i;
        col_diff_l[i] = col_diff_l[128 - i] = k * 100 * 100;
        // col_diff_r[i] = col_diff_r[128 - i] = k * 30 * 30;
        // col_diff_g[i] = col_diff_g[128 - i] = k * 59 * 59;
        // col_diff_b[i] = col_diff_b[128 - i] = k * 11 * 11;
        col_diff_a[i] = col_diff_a[128 - i] = k * 8 * 8;
    }
}

int Palette::findBestfit(int l, int a) const {
    l >>= 3;
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

        LA8 la = m_colors[i];

        int dl = ((la.l >> 3) - l) & 127;
        int da = ((la.a >> 3) - a) & 127;

        int coldiff = col_diff_l[dl] + col_diff_a[da];

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
