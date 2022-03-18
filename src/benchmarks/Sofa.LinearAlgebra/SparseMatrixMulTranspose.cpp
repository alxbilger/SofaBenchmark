#include <iostream>
#include <benchmark/benchmark.h>
#include <utils/RandomValuePool.h>
#include <sofa/linearalgebra/CompressedRowSparseMatrix.h>
#include <Eigen/Sparse>

constexpr int64_t minMatrixSize = 1 << 9;
constexpr int64_t maxMatrixSize = 1 << 12;
constexpr int64_t nbMaxNonZeros = maxMatrixSize * maxMatrixSize * 150 / 1000 + 1;

template<class TReal>
static void BM_SparseMatrixMulTranspose_Eigen(benchmark::State& state)
{
    const auto matrixSize = state.range(0);
    const auto sparsityPerMil = state.range(1);
    const auto sparsity = static_cast<TReal>(sparsityPerMil) / 1000.; //since only integers can be passed as an argument, a number between 0 and 1000 must be provided
    const auto nbNonZero = static_cast<Eigen::Index>(sparsity * static_cast<TReal>(matrixSize*matrixSize));

    const auto& values_a = RandomValuePool<TReal, nbMaxNonZeros>::get();
    const auto& values_b = RandomValuePool<TReal, nbMaxNonZeros + 1>::get();

    const auto& indices_a_x = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros>::get();
    const auto& indices_a_y = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros+1>::get();
    const auto& indices_b_x = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros+2>::get();
    const auto& indices_b_y = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros+3>::get();

    Eigen::SparseMatrix<TReal> matrix_a, matrix_b;
    matrix_a.resize(matrixSize, matrixSize);
    matrix_b.resize(matrixSize, matrixSize);

    {
        sofa::type::vector<Eigen::Triplet<TReal> > triplets_a;
        for (Eigen::Index i = 0; i < nbNonZero; ++i)
        {
            triplets_a.emplace_back(indices_a_x[i], indices_a_y[i], values_a[i]);
        }
        matrix_a.setFromTriplets(triplets_a.begin(), triplets_a.end());
    }
    {
        sofa::type::vector<Eigen::Triplet<TReal> > triplets_b;
        for (Eigen::Index i = 0; i < nbNonZero; ++i)
        {
            triplets_b.emplace_back(indices_b_x[i], indices_b_y[i], values_b[i]);
        }
        matrix_b.setFromTriplets(triplets_b.begin(), triplets_b.end());
    }

    for (auto _ : state)
    {
        Eigen::SparseMatrix<TReal> res;
        benchmark::DoNotOptimize(res = matrix_a.transpose() * matrix_b);
    }

    state.counters["nbNonZero"] = benchmark::Counter(nbNonZero);
}

template<class TReal>
static void BM_SparseMatrixMulTranspose_CRS(benchmark::State& state)
{
    const auto matrixSize = state.range(0);
    const auto sparsityPerMil = state.range(1);
    const auto sparsity = static_cast<TReal>(sparsityPerMil) / 1000.; //since only integers can be passed as an argument, a number between 0 and 1000 must be provided
    const auto nbNonZero = static_cast<sofa::SignedIndex>(sparsity * static_cast<TReal>(matrixSize*matrixSize));

    const auto& values_a = RandomValuePool<TReal, nbMaxNonZeros>::get();
    const auto& values_b = RandomValuePool<TReal, nbMaxNonZeros + 1>::get();

    const auto& indices_a_x = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros>::get();
    const auto& indices_a_y = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros+1>::get();
    const auto& indices_b_x = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros+2>::get();
    const auto& indices_b_y = RandomValuePool<sofa::SignedIndex, nbMaxNonZeros+3>::get();

    sofa::linearalgebra::CompressedRowSparseMatrix<TReal> matrix_a, matrix_b;

    matrix_a.resize(matrixSize, matrixSize);
    matrix_b.resize(matrixSize, matrixSize);

    for (unsigned int i = 0; i < nbNonZero; ++i)
    {
        matrix_a.add(indices_a_x[i], indices_a_y[i], values_a[i]);
        matrix_b.add(indices_b_x[i], indices_b_y[i], values_b[i]);
    }

    matrix_a.compress();
    matrix_b.compress();

    for (auto _ : state)
    {
        sofa::linearalgebra::CompressedRowSparseMatrix<TReal> res;
        matrix_a.mulTranspose(res, matrix_b);
    }

    state.counters["nbNonZero"] = benchmark::Counter(nbNonZero);
}


BENCHMARK_TEMPLATE(BM_SparseMatrixMulTranspose_Eigen, SReal)->ArgsProduct({benchmark::CreateRange(minMatrixSize, maxMatrixSize, 2), {10, 20, 100, 150}})->Unit(benchmark::kMicrosecond);
BENCHMARK_TEMPLATE(BM_SparseMatrixMulTranspose_CRS, SReal)->ArgsProduct({benchmark::CreateRange(minMatrixSize, maxMatrixSize, 2), {10, 20, 100, 150}})->Unit(benchmark::kMicrosecond);
