#include <iostream>
#include <benchmark/benchmark.h>
#include <utils/RandomValuePool.h>
#include <sofa/linearalgebra/CompressedRowSparseMatrix.h>
#include <Eigen/Sparse>

constexpr int64_t minMatrixSize = 1 << 9;
constexpr int64_t maxMatrixSize = 1 << 12;
constexpr int64_t nbMaxNonZeros = maxMatrixSize * maxMatrixSize * 150 / 1000 + 1;

template<class TReal>
static void BM_SparseMatrixCompression_Eigen(benchmark::State& state)
{
    const auto matrixSize = state.range(0);
    const auto sparsityPerMil = state.range(1);
    const auto sparsity = static_cast<TReal>(sparsityPerMil) / 1000.; //since only integers can be passed as an argument, a number between 0 and 1000 must be provided
    const auto nbNonZero = static_cast<Eigen::Index>(sparsity * static_cast<TReal>(matrixSize*matrixSize));

    const auto& values = RandomValuePool<TReal, nbMaxNonZeros>::get();

    const auto& indices_x = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros>::get();
    const auto& indices_y = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros+1>::get(); //not the same size to generate other values

    for (auto _ : state)
    {
        Eigen::SparseMatrix<TReal> matrix;
        matrix.resize(matrixSize, matrixSize);

        sofa::type::vector<Eigen::Triplet<TReal> > triplets;
        for (Eigen::Index i = 0; i < nbNonZero; ++i)
        {
            triplets.emplace_back(indices_x[i], indices_y[i], values[i]);
        }
        matrix.setFromTriplets(triplets.begin(), triplets.end());
    }

    state.counters["nbNonZero"] = benchmark::Counter(nbNonZero);
}

template<class TReal>
static void BM_SparseMatrixCompression_CRS(benchmark::State& state)
{
    const auto matrixSize = state.range(0);
    const auto sparsityPerMil = state.range(1);
    const auto sparsity = static_cast<TReal>(sparsityPerMil) / 1000.; //since only integers can be passed as an argument, a number between 0 and 1000 must be provided
    const auto nbNonZero = static_cast<sofa::SignedIndex>(sparsity * static_cast<TReal>(matrixSize*matrixSize));

    const auto& values = RandomValuePool<TReal, nbMaxNonZeros>::get();

    const auto& indices_x = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros>::get();
    const auto& indices_y = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros+1>::get(); //not the same size to generate other values

    for (auto _ : state)
    {
        sofa::linearalgebra::CompressedRowSparseMatrix<TReal> matrix;

        matrix.resize(matrixSize, matrixSize);

        for (unsigned int i = 0; i < nbNonZero; ++i)
        {
            matrix.add(indices_x[i], indices_y[i], values[i]);
        }

        matrix.compress();
    }

    state.counters["nbNonZero"] = benchmark::Counter(nbNonZero);
}


BENCHMARK_TEMPLATE(BM_SparseMatrixCompression_Eigen, SReal)->ArgsProduct({benchmark::CreateRange(minMatrixSize, maxMatrixSize, 2), {10, 20, 100, 150}})->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(BM_SparseMatrixCompression_CRS, SReal)->ArgsProduct({benchmark::CreateRange(minMatrixSize, maxMatrixSize, 2), {10, 20, 100, 150}})->Unit(benchmark::kMicrosecond);
