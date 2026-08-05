#pragma once
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/ref_counted.hpp>

namespace gd = godot;

class Palette;
class ErrorDiffusionDither;

class DitherProcessor : public gd::RefCounted {
    GDCLASS(DitherProcessor, gd::RefCounted)

public:
    enum DitherMethod {
        DITHER_ERROR_DIFFUSION,
    };

protected:
    static void _bind_methods();

public:
    gd::Ref<gd::Image> process(float factor, gd::Ref<gd::Image> source, DitherMethod method);
};

VARIANT_ENUM_CAST(DitherProcessor::DitherMethod);
