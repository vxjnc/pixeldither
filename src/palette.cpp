#include "palette.hpp"

#include <limits>

void Palette::addEntry(const gd::Color& color) { m_colors.push_back(color); }

int Palette::size() const { return (int)m_colors.size(); }

gd::Color Palette::getEntry(int i) const { return m_colors[i]; }

void Palette::initBestfit() {
    col_diff.resize(4 * 128, 0);
    col_diff_g = &col_diff[128 * 0];
    col_diff_r = &col_diff[128 * 1];
    col_diff_b = &col_diff[128 * 2];
    col_diff_a = &col_diff[128 * 3];

    for (int i = 1; i < 64; ++i) {
        int k = i * i;
        col_diff_g[i] = col_diff_g[128 - i] = k * 59 * 59;
        col_diff_r[i] = col_diff_r[128 - i] = k * 30 * 30;
        col_diff_b[i] = col_diff_b[128 - i] = k * 11 * 11;
        col_diff_a[i] = col_diff_a[128 - i] = k * 8 * 8;
    }
}

int Palette::findBestfit(int r, int g, int b, int a, int mask_index) const {
    r >>= 3;
    g >>= 3;
    b >>= 3;
    a >>= 3;

    if (a == 0 && mask_index >= 0) {
        return mask_index;
    }

    int bestfit = 0;
    int lowest = std::numeric_limits<int>::max();

    for (int i = 0; i < m_colors.size(); ++i) {
        const gd::Color& rgba = m_colors[i];

        int coldiff = col_diff_g[((rgba.get_g8() >> 3) - g) & 127];
        if (coldiff < lowest) {
            coldiff += col_diff_r[((rgba.get_r8() >> 3) - r) & 127];
            if (coldiff < lowest) {
                coldiff += col_diff_b[((rgba.get_b8() >> 3) - b) & 127];
                if (coldiff < lowest) {
                    coldiff += col_diff_a[((rgba.get_a8() >> 3) - a) & 127];
                    if (coldiff < lowest && i != mask_index) {
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
