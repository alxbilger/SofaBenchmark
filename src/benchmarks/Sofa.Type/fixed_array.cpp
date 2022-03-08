#include <benchmark/benchmark.h>

#include <utils/RandomValuePool.h>
#include <sofa/type/fixed_array.h>
#include <sofa/type/Vec.h>
#include <array>

template <typename Container>
static void BM_FixedArray_defaultconstruct(benchmark::State& state);
static void BM_FixedArray_defaultconstruct_vec3noinit(benchmark::State& state);

template <typename Container>
static void BM_FixedArray_construct(benchmark::State& state);

template <typename Container>
static void BM_FixedArray_constantAssignment(benchmark::State& state);

template <typename Container>
static void BM_FixedArray_clear(benchmark::State& state);

using stdarray3f = std::array<float, 3>;
using sofatypefixedarray3f = sofa::type::fixed_array<float, 3>;

constexpr int64_t minSubIterations = 8 << 10;
constexpr int64_t maxSubIterations = 8 << 12;

BENCHMARK_TEMPLATE1(BM_FixedArray_defaultconstruct, stdarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_FixedArray_defaultconstruct, sofatypefixedarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_FixedArray_defaultconstruct, sofa::type::Vec3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_FixedArray_defaultconstruct_vec3noinit)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

BENCHMARK_TEMPLATE1(BM_FixedArray_construct, stdarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_FixedArray_construct, sofatypefixedarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_FixedArray_construct, sofa::type::Vec3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

BENCHMARK_TEMPLATE1(BM_FixedArray_constantAssignment, stdarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} });
BENCHMARK_TEMPLATE1(BM_FixedArray_constantAssignment, sofatypefixedarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} });
BENCHMARK_TEMPLATE1(BM_FixedArray_constantAssignment, sofa::type::Vec3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} });

BENCHMARK_TEMPLATE1(BM_FixedArray_clear, sofa::type::Vec3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} });

template <typename Container>
void BM_FixedArray_defaultconstruct(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(Container{ });
        }
    }
}

void BM_FixedArray_defaultconstruct_vec3noinit(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(sofa::type::Vec3f { sofa::type::NOINIT });
        }
    }
}

template <typename Container>
void BM_FixedArray_construct(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 3;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(Container{values[i], values[i+1], values[i+2]});
        }
    }
}

template <typename Container>
void BM_FixedArray_constantAssignment(benchmark::State& state)
{
    Container c;
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(c = Container());
        }
    }
}

template <typename Container>
void BM_FixedArray_clear(benchmark::State& state)
{
    Container c;
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            c.clear();
        }
    }
}
