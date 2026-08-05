#include "image_utils.hpp"

#include <cstring>
#include <stdexcept>

#include <stb_image.h>
#include <stb_image_write.h>

Image load_png(const std::filesystem::path& path) {
    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_grey_alpha);

    if (!data) {
        throw std::runtime_error("Failed to load image: " + path.string());
    }

    Image image{
        .width = width,
        .height = height,
        .pixels = std::vector<LA8>(width * height),
    };

    std::memcpy(image.pixels.data(), data, image.pixels.size() * sizeof(LA8));

    stbi_image_free(data);

    return image;
}

void save_png(const std::filesystem::path& path, const Image& image) {
    if (!stbi_write_png(path.string().c_str(), image.width, image.height, STBI_grey_alpha,
                        image.pixels.data(), image.width * sizeof(LA8))) {
        throw std::runtime_error("Failed to save image: " + path.string());
    }
}
