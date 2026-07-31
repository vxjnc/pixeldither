#include "dither.hpp"

#include "error_diffusion.hpp"
#include "palette.hpp"

static void dither_rgb_image_to_indexed(ErrorDiffusionDither& algorithm, double factor,
                                        const gd::Ref<gd::Image>& srcImage, gd::Ref<gd::Image>& dstImage,
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
            dstImage->set_pixel(x, y, palette.getEntry(index));
        }
    }
}

void DitherProcessor::_bind_methods() {
    gd::ClassDB::bind_method(gd::D_METHOD("process", "source"), &DitherProcessor::process);
}

gd::Ref<gd::Image> DitherProcessor::process(gd::Ref<gd::Image> source) {
    ERR_FAIL_COND_V_MSG(source->get_format() != gd::Image::FORMAT_RGBA8, gd::Ref<gd::Image>(),
                        "DitherProcessor::process: source image must be in FORMAT_RGBA8");

    const gd::Color palette_colors[] = {
        gd::Color(0, 0, 0, 1),
        gd::Color(1, 1, 1, 1),
        gd::Color(0, 0, 0, 0),
    };
    Palette palette(palette_colors);
    constexpr int transparentIndex = 2;

    gd::Ref<gd::Image> dstImage =
        gd::Image::create(source->get_width(), source->get_height(), false, gd::Image::FORMAT_LA8);

    ErrorDiffusionDither dither(transparentIndex);
    dither_rgb_image_to_indexed(dither, 1.0, source, dstImage, palette);

    return dstImage;
}
