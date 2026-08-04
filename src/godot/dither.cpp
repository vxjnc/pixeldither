#include "dither.hpp"

#include "core/error_diffusion.hpp"

void DitherProcessor::_bind_methods() {
    gd::ClassDB::bind_method(gd::D_METHOD("process", "source"), &DitherProcessor::process);
}

gd::Ref<gd::Image> DitherProcessor::process(gd::Ref<gd::Image> source) {
    ERR_FAIL_COND_V_MSG(source->get_format() != gd::Image::FORMAT_LA8, gd::Ref<gd::Image>(),
                        "DitherProcessor::process: source image must be in FORMAT_LA8");

    int w = source->get_width();
    int h = source->get_height();

    gd::PackedByteArray src_data = source->get_data();
    const LA8* src_ptr = reinterpret_cast<const LA8*>(src_data.ptr());

    gd::PackedByteArray dst_data;
    dst_data.resize(w * h * sizeof(LA8));
    LA8* dst_ptr = reinterpret_cast<LA8*>(dst_data.ptrw());

    ErrorDiffusionDither dither;
    dither.dither_image_to_indexed(1.0, src_ptr, w, h, dst_ptr);

    gd::Ref<gd::Image> result = gd::Image::create_from_data(w, h, false, gd::Image::FORMAT_LA8, dst_data);

    return result;
}
