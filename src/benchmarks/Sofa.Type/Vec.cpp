#include <benchmark/benchmark.h>

#include <utils/RandomValuePool.h>

#include <sofa/type/fixed_array.h>
#include <sofa/type/fixed_array_algorithms.h>
#include <sofa/type/Vec.h>
#include <array>
#include <numeric>
#include <sofa/type/vector.h>
#include <cstring>

static void BM_Vec_dot(benchmark::State& state);

template <typename Container>
static void BM_Vec_stdinnerproduct(benchmark::State& state);

static void BM_Vec_InitSimpleLoop(benchmark::State& state);
static void BM_Vec_InitMemset(benchmark::State& state);

static void BM_Vec_CopySimpleEqual(benchmark::State& state);
static void BM_Vec_CopySimpleLoop(benchmark::State& state);
static void BM_Vec_CopyStdCopy(benchmark::State& state);

using stdarray3f = std::array<float, 3>;
using sofatypefixedarray3f = sofa::type::fixed_array<float, 3>;

constexpr int64_t minSubIterations = 8 << 4;
constexpr int64_t maxSubIterations = 8 << 6;

BENCHMARK(BM_Vec_dot)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_Vec_stdinnerproduct, stdarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_Vec_stdinnerproduct, sofatypefixedarray3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE1(BM_Vec_stdinnerproduct, sofa::type::Vec3f)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Vec_InitSimpleLoop)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Vec_InitMemset)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Vec_CopyStdCopy)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Vec_CopySimpleLoop)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Vec_CopySimpleEqual)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

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

void BM_Vec_InitSimpleLoop(benchmark::State& state)
{
    using Container = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    sofa::type::vector<Container> list;
    list.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0) * 3; i+=3)
    {
        list.emplace_back(values[i], values[i+1], values[i+2]);
    }

    for (auto _ : state)
    {
        for (auto& v : list)
        {
            v.clear();
        }
    }
}

void BM_Vec_InitMemset(benchmark::State& state)
{
    using Container = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    sofa::type::vector<Container> list;
    list.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0) * 3; i+=3)
    {
        list.emplace_back(values[i], values[i+1], values[i+2]);
    }

    for (auto _ : state)
    {
        std::memset(list.data(), 0, sizeof(Container::value_type) * Container::total_size * list.size());
    }
}

void BM_Vec_CopyStdCopy(benchmark::State& state)
{
    using Container = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 * 2 + 1;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    sofa::type::vector<Container> list;
    list.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0) * 3; i+=3)
    {
        list.emplace_back(values[i], values[i+1], values[i+2]);
    }

    sofa::type::vector<Container> b;
    b.resize(list.size());

    for (auto _ : state)
    {
        float* valuePtr = list.begin()->array();
        std::copy(valuePtr, valuePtr + 3 * list.size(), b.begin()->array());
    }
}

void BM_Vec_CopySimpleLoop(benchmark::State& state)
{
    using Container = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 * 2 + 1;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    sofa::type::vector<Container> list;
    list.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0) * 3; i+=3)
    {
        list.emplace_back(values[i], values[i+1], values[i+2]);
    }

    sofa::type::vector<Container> b;
    b.resize(list.size());

    for (auto _ : state)
    {
        for (unsigned int i=0; i<list.size(); i++)
            b[i] = list[i];
    }
}

void BM_Vec_CopySimpleEqual(benchmark::State& state)
{
    using Container = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 * 2 + 1;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    sofa::type::vector<Container> list;
    list.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0) * 3; i+=3)
    {
        list.emplace_back(values[i], values[i+1], values[i+2]);
    }

    sofa::type::vector<Container> b;
    b.resize(list.size());

    for (auto _ : state)
    {
        b = list;
    }
}
