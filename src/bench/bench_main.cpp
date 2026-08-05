#include <cstdint>
#include <vector>

#include <benchmark/benchmark.h>

#include "core/error_diffusion.hpp"
#include "core/la8.hpp"
#include "core/old_dithering.hpp"
#include "core/ordered_dithering.hpp"

template <typename DitherT> class DitherBenchmark : public benchmark::Fixture {
public:
    int width = 0;
    int height = 0;
    std::vector<LA8> src_pixels;
    std::vector<LA8> dst_pixels;
    DitherT dither;

    void SetUp(const ::benchmark::State& state) override {
        const int dim = static_cast<int>(state.range(0));
        width = dim;
        height = dim;

        src_pixels.resize(width * height);
        dst_pixels.resize(width * height);

        for (size_t i = 0; i < src_pixels.size(); ++i) {
            src_pixels[i].l = static_cast<uint8_t>(i % 256);
            src_pixels[i].a = 255;
        }
    }

    void TearDown(const ::benchmark::State&) override {
        src_pixels.clear();
        src_pixels.shrink_to_fit();
        dst_pixels.clear();
        dst_pixels.shrink_to_fit();
    }
};

using ErrorDiffusionBenchmark = DitherBenchmark<ErrorDiffusionDither>;
using OldDitheringBenchmark = DitherBenchmark<OldDithering>;
using OrderedDitheringBenchmark = DitherBenchmark<OrderedDithering>;

BENCHMARK_DEFINE_F(ErrorDiffusionBenchmark, Process)(benchmark::State& state) {
    for (auto _ : state) {
        dither.dither_image_to_indexed(1.0, src_pixels.data(), width, height, dst_pixels.data());

        benchmark::DoNotOptimize(src_pixels.data());
        benchmark::DoNotOptimize(dst_pixels.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * width * height);
    state.SetBytesProcessed(state.iterations() * width * height * sizeof(LA8));
    state.counters["Dimension"] = static_cast<double>(width);
}

BENCHMARK_DEFINE_F(OldDitheringBenchmark, Process)(benchmark::State& state) {
    for (auto _ : state) {
        dither.dither_image_to_indexed(1.0, src_pixels.data(), width, height, dst_pixels.data());

        benchmark::DoNotOptimize(src_pixels.data());
        benchmark::DoNotOptimize(dst_pixels.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * width * height);
    state.SetBytesProcessed(state.iterations() * width * height * sizeof(LA8));
    state.counters["Dimension"] = static_cast<double>(width);
}

BENCHMARK_DEFINE_F(OrderedDitheringBenchmark, Process)(benchmark::State& state) {
    for (auto _ : state) {
        dither.dither_image_to_indexed(1.0, src_pixels.data(), width, height, dst_pixels.data());

        benchmark::DoNotOptimize(src_pixels.data());
        benchmark::DoNotOptimize(dst_pixels.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * width * height);
    state.SetBytesProcessed(state.iterations() * width * height * sizeof(LA8));
    state.counters["Dimension"] = static_cast<double>(width);
}

BENCHMARK_REGISTER_F(ErrorDiffusionBenchmark, Process)
    ->Unit(benchmark::kNanosecond)
    ->RangeMultiplier(2)
    ->Range(64, 2048);

BENCHMARK_REGISTER_F(OldDitheringBenchmark, Process)
    ->Unit(benchmark::kNanosecond)
    ->RangeMultiplier(2)
    ->Range(64, 2048);

BENCHMARK_REGISTER_F(OrderedDitheringBenchmark, Process)
    ->Unit(benchmark::kNanosecond)
    ->RangeMultiplier(2)
    ->Range(64, 2048);

BENCHMARK_MAIN();
