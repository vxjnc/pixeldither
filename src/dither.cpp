#include "dither.hpp"

#include "error_diffusion.hpp"
#include "palette.hpp"

static void dither_rgb_image_to_indexed(ErrorDiffusionDither& algorithm, double factor,
                                        const gd::Ref<gd::Image>& srcImage, uint8_t* dstData,
                                        const Palette& palette) {
    const int w = srcImage->get_width();
    const int h = srcImage->get_height();

    algorithm.start(srcImage, factor);

    for (int y = 0; y < h; ++y) {
        int start = (y & 1) ? w - 1 : 0;
        int stop = (y & 1) ? -1 : w;
        int step = (y & 1) ? -1 : 1;

        for (int x = start; x != stop; x += step) {
            int index = algorithm.ditherRgbToIndex2D(x, y, palette);
            const gd::Color& c = palette.getEntry(index);

            uint8_t* out = &dstData[(y * w + x) * 2]; // FORMAT_LA8
            out[0] = c.get_r8();
            out[1] = c.get_a8();
        }
    }
}

void DitherProcessor::_bind_methods() {
    gd::ClassDB::bind_method(gd::D_METHOD("process", "source"), &DitherProcessor::process);
}

gd::Ref<gd::Image> DitherProcessor::process(gd::Ref<gd::Image> source) {
    ERR_FAIL_COND_V_MSG(source->get_format() != gd::Image::FORMAT_RGBA8, gd::Ref<gd::Image>(),
                        "DitherProcessor::process: source image must be in FORMAT_RGBA8");

    Palette palette;

    int w = source->get_width();
    int h = source->get_height();

    gd::PackedByteArray dst_data;
    dst_data.resize(w * h * 2); // FORMAT_LA8
    uint8_t* dst_ptr = dst_data.ptrw();

    ErrorDiffusionDither dither;
    dither_rgb_image_to_indexed(dither, 1.0, source, dst_ptr, palette);

    return gd::Image::create_from_data(w, h, false, gd::Image::FORMAT_LA8, dst_data);
}
