#include "dither.hpp"

#include "error_diffusion.hpp"
#include "palette.hpp"

void DitherProcessor::_bind_methods() {
    gd::ClassDB::bind_method(gd::D_METHOD("process", "source"), &DitherProcessor::process);
}

gd::Ref<gd::Image> DitherProcessor::process(gd::Ref<gd::Image> source) {
    ERR_FAIL_COND_V_MSG(source->get_format() != gd::Image::FORMAT_RGBA8, gd::Ref<gd::Image>(),
                        "DitherProcessor::process: source image must be in FORMAT_RGBA8");

    auto t0 = std::chrono::high_resolution_clock::now();

    Palette palette;
    int w = source->get_width();
    int h = source->get_height();

    gd::PackedByteArray src_data = source->get_data();
    const uint8_t* src_ptr = src_data.ptr();

    gd::PackedByteArray dst_data;
    dst_data.resize(w * h * 2);
    uint8_t* dst_ptr = dst_data.ptrw();

    ErrorDiffusionDither dither;
    dither_rgb_image_to_indexed(dither, 1.0, src_ptr, w, h, dst_ptr, palette);

    gd::Ref<gd::Image> result = gd::Image::create_from_data(w, h, false, gd::Image::FORMAT_LA8, dst_data);

    return result;
}

void DitherProcessor::dither_rgb_image_to_indexed(ErrorDiffusionDither& algorithm, double factor,
                                                  const uint8_t* srcData, int width, int height,
                                                  uint8_t* dstData, const Palette& palette) {
    algorithm.start(srcData, width, height, factor);

    for (int y = 0; y < height; ++y) {
        algorithm.beginRow(y);

        int start = (y & 1) ? width - 1 : 0;
        int stop = (y & 1) ? -1 : width;
        int step = (y & 1) ? -1 : 1;

        for (int x = start; x != stop; x += step) {
            int index = algorithm.ditherRgbToIndex2D(x, y, palette);
            RGBA8 c = palette.getEntry(index);

            uint8_t* out = &dstData[(y * width + x) * 2]; // FORMAT_LA8
            out[0] = c.r;
            out[1] = c.a;
        }
    }
}
