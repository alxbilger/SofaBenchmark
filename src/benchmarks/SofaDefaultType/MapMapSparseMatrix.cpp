#include <benchmark/benchmark.h>
#include <sofa/defaulttype/VecTypes.h>
#include <utils/RandomValuePool.h>

using MapMapSparseMatrix = sofa::defaulttype::Vec3Types::MatrixDeriv;

constexpr int64_t minSubIterations = 1 << 9;
constexpr int64_t maxSubIterations = 1 << 12;

static void BM_MapMapSparseMatrix_writeLine(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const auto& lines = RandomValuePool<MapMapSparseMatrix::KeyType, totalsize>::get();

    MapMapSparseMatrix matrix;

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            matrix.writeLine(lines[i]);
        }
    }
}

static void BM_MapMapSparseMatrix_addCol(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations;
    const auto& lines = RandomValuePool<MapMapSparseMatrix::KeyType, totalsize>::get();
    const auto& cols = RandomValuePool<MapMapSparseMatrix::KeyType, totalsize + 1>::get();

    MapMapSparseMatrix matrix;

    MapMapSparseMatrix::Data vec;
    std::transform(vec.begin(), vec.end(), vec.begin(), [](const auto& el) { return el + 1; });

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            auto it = matrix.writeLine(lines[i]);

            for (unsigned int j = 0; j < state.range(1); ++j)
            {
                it.addCol(cols[j], vec);
            }
        }
    }
}

BENCHMARK(BM_MapMapSparseMatrix_writeLine)->RangeMultiplier(2)->Range(minSubIterations, maxSubIterations)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_MapMapSparseMatrix_addCol)
    ->RangeMultiplier(2)
    ->Ranges({{minSubIterations, maxSubIterations}, {minSubIterations, maxSubIterations}})
    ->Unit(benchmark::kMillisecond);