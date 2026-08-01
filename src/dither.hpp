#pragma once
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace gd = godot;

class Palette;
class ErrorDiffusionDither;

class DitherProcessor : public gd::RefCounted {
    GDCLASS(DitherProcessor, gd::RefCounted)

    void dither_rgb_image_to_indexed(ErrorDiffusionDither& algorithm, double factor, const uint8_t* srcData,
                                     int width, int height, uint8_t* dstData, const Palette& palette);

protected:
    static void _bind_methods();

public:
    gd::Ref<gd::Image> process(gd::Ref<gd::Image> source);
};
