#pragma once

#include <algorithm>
#include <filesystem>
#include <vector>

#include "core/la8.hpp"

struct Image {
    int width;
    int height;
    std::vector<LA8> pixels;

    bool operator==(const Image& other) const {
        return width == other.width && height == other.height && std::ranges::equal(pixels, other.pixels);
    }
};

Image load_png(const std::filesystem::path& path);
void save_png(const std::filesystem::path& path, const Image& image);
