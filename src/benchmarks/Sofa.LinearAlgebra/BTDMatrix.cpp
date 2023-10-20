#include <benchmark/benchmark.h>

#include <sofa/linearalgebra/BTDMatrix.h>
#include <utils/RandomValuePool.h>

constexpr int64_t maxSubIterations = 8 << 7;

template<std::size_t N, typename T>
static void BM_BTDMatrix_add(benchmark::State& state)
{
    sofa::linearalgebra::BTDMatrix<N, T> matrix;

    constexpr auto totalsize = maxSubIterations * N * N;
    const std::array<T, totalsize>& values = RandomValuePool<T, totalsize>::get();

    std::vector<sofa::type::Mat<N, N, T>> matricesToAdd;
    matricesToAdd.resize(state.range(0));
    for (int64_t i = 0; i < state.range(0); i++)
    {
        for (sofa::Index j = 0; j < N; ++j)
        {
            for (sofa::Index k = 0; k < N; ++k)
            {
                matricesToAdd[i][j][k] = values[i * N * N + j * N + k];
            }
        }
    }

    auto randIndices = RandomValuePool<sofa::Index, maxSubIterations * 2>::get();
    for (auto& id : randIndices)
    {
        id *= N;
    }
    std::vector<std::pair<sofa::Index, sofa::Index>> indices;
    indices.reserve(state.range(0));
    for (int64_t i = 0; i < state.range(0) * 2; i += 2)
    {
        indices.emplace_back(randIndices[i], randIndices[i+1]);
    }

    const auto max = *std::max_element(randIndices.begin(), randIndices.end());
    matrix.resize(N * ((max+N) / N), N * ((max+N) / N));

    for (auto _ : state)
    {
        for (int64_t i = 0; i < state.range(0); ++i)
        {
            matrix.add(indices[i].first, indices[i].second, matricesToAdd[i]);
        }
    }
}

BENCHMARK_TEMPLATE(BM_BTDMatrix_add, 6, SReal)->Range(8 << 3, maxSubIterations);
