#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <vector>

template <size_t Channels> struct ErrorBuffer {
    std::array<std::vector<int>, Channels> curr;
    std::array<std::vector<int>, Channels> next;

    void resize(size_t width) {
        for (size_t ch = 0; ch < Channels; ++ch) {
            curr[ch].resize(width, 0);
            next[ch].resize(width, 0);
        }
    }

    void swapAndResetNext() {
        curr.swap(next);

        for (size_t ch = 0; ch < Channels; ++ch) {
            std::ranges::fill(next[ch], 0);
        }
    }
};
