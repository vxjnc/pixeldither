#include "image_utils.hpp"

#include <format>
#include <functional>

#include <doctest/doctest.h>

#include "core/error_diffusion.hpp"
#include "core/old_dithering.hpp"
#include "core/ordered_dithering.hpp"

namespace {
    struct TestCase {
        const char* name;
        std::function<void(const Image&, Image&)> run;
    };

    const TestCase cases[] = {
        {"error_diffusion",
         [](const Image& input, Image& output) {
             ErrorDiffusionDither{}.dither_image_to_indexed(1.0f, input.pixels.data(), input.width,
                                                            input.height, output.pixels.data());
         }},
        {"ordered_dithering",
         [](const Image& input, Image& output) {
             OrderedDithering{}.dither_image_to_indexed(1.0f, input.pixels.data(), input.width, input.height,
                                                        output.pixels.data());
         }},
        {"old_dithering", [](const Image& input, Image& output) {
             OldDithering{}.dither_image_to_indexed(1.0f, input.pixels.data(), input.width, input.height,
                                                    output.pixels.data());
         }}};
}

TEST_CASE("Reference images") {
    auto input = load_png("tests/input/example.png");

    for (const auto& tc : cases) {
        INFO(tc.name);

        Image output{input.width, input.height, std::vector<LA8>(input.pixels.size())};

        tc.run(input, output);

        auto expected = load_png(std::format("tests/expected/{}.png", tc.name));

        CHECK(output == expected);
    }
}
