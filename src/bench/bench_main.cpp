#include <cstdint>
#include <ranges>
#include <vector>

#include <benchmark/benchmark.h>

#include "core/error_diffusion.hpp"
#include "core/palette.hpp"
#include "core/rgba8.hpp"

static void BM_ErrorDiffusion(benchmark::State& state) {
    const int width = static_cast<int>(state.range(0));
    const int height = static_cast<int>(state.range(1));

    std::vector<LA8> src_pixels(width * height);
    std::vector<LA8> dst_pixels(width * height);

    for (size_t i = 0; i < src_pixels.size(); ++i) {
        src_pixels[i].l = static_cast<uint8_t>(i % 256);
        src_pixels[i].a = 255;
    }

    Palette palette;
    ErrorDiffusionDither dither;

    for (auto _ : state) {
        dither.dither_image_to_indexed(1.0, src_pixels.data(), width, height, dst_pixels.data(), palette);

        benchmark::DoNotOptimize(src_pixels.data());
        benchmark::DoNotOptimize(dst_pixels.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * width * height);
    state.SetBytesProcessed(state.iterations() * width * height * sizeof(LA8));
}

BENCHMARK(BM_ErrorDiffusion)
    ->Args({64, 64})
    ->Args({128, 128})
    ->Args({256, 256})
    ->Args({512, 512})
    ->Args({1024, 1024})
    ->Args({2048, 2048})
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
