#include <benchmark/benchmark.h>

#include <utils/RandomValuePool.h>

#include <sofa/type/fixed_array.h>
#include <sofa/type/fixed_array_algorithms.h>
#include <sofa/type/Vec.h>
#include <array>
#include <numeric>

static void BM_Vec_dot(benchmark::State& state);

template <typename Container>
static void BM_Vec_stdinnerproduct(benchmark::State& state);

using stdarray3f = std::array<float, 3>;
using sofatypefixedarray3f = sofa::type::fixed_array<float, 3>;

constexpr int64_t minSubIterations = 8 << 4;
constexpr int64_t maxSubIterations = 8 << 6;

BENCHMARK(BM_Vec_dot)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_Vec_stdinnerproduct, stdarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_Vec_stdinnerproduct, sofatypefixedarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_Vec_stdinnerproduct, sofa::type::Vec3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

// test on dot() for sofa::type::Vec
void BM_Vec_dot(benchmark::State& state)
{
    using Container = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<Container> vc1(state.range(0));
        std::vector<Container> vc2(state.range(0));

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            vc1.emplace_back(Container{ values[i*6 + 0], values[i * 6 + 1], values[i * 6 + 2] });
            vc2.emplace_back(Container{ values[i*6 + 3], values[i * 6 + 4], values[i * 6 + 5] });
        }
        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(sofa::type::dot(vc1[i], vc2[i]));
        }
    }
}

// test on the generic dot() (aka inner_product) from the stl
template <typename Container>
static void BM_Vec_stdinnerproduct(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 3 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<Container> vc1(state.range(0));
        std::vector<Container> vc2(state.range(0));

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            vc1.emplace_back(Container{ values[i * 6 + 0], values[i * 6 + 1], values[i * 6 + 2] });
            vc2.emplace_back(Container{ values[i * 6 + 3], values[i * 6 + 4], values[i * 6 + 5] });
        }
        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(std::inner_product(std::cbegin(vc1[i]), std::cend(vc1[i]), std::cbegin(vc2[i]), static_cast<float>(0)));
        }
    }
}
