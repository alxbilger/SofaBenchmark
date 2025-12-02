#include <benchmark/benchmark.h>
#include <sofa/type/MatSym.h>

static void BM_MatSym_operator_access(benchmark::State& state)
{
    sofa::type::MatSym<3, double> m{ 1., 2., 3., 4., 5., 6. };
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            for (sofa::Size j = 0; j < 3; ++j)
            {
                for (sofa::Size k = 0; k < 3; ++k)
                {
                    benchmark::DoNotOptimize(m(j, k));
                }
            }
        }
    }
}

static void BM_MatSym_matvecproduct(benchmark::State& state)
{
    sofa::type::MatSym<3, double> m{ 1., 2., 3., 4., 5., 6. };

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            sofa::type::Vec3d v{ i, i, i };
            benchmark::DoNotOptimize(m * v);
        }
    }
}

constexpr int64_t minSubIterations = 8 << 4;
constexpr int64_t maxSubIterations = 8 << 7;
BENCHMARK(BM_MatSym_operator_access)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_MatSym_matvecproduct)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
