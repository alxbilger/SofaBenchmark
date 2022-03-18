#include <iostream>
#include <benchmark/benchmark.h>

#include <sofa/linearalgebra/SparseMatrixProduct[EigenSparseMatrix].h>
#include <utils/SparseMatrix.h>



template<class TMatrix>
class BM_SparseMatrixProduct : public benchmark::Fixture
{
public:

    void SetUp(const ::benchmark::State& state) override
    {
        const auto matrixSize = state.range(0);
        const auto sparsityPerMil = state.range(1);
        const auto sparsity = static_cast<SReal>(sparsityPerMil) / 1000.; //since only integers can be passed as an argument, a number between 0 and 1000 must be provided

        auto itA = matrixMapA.find({matrixSize, sparsityPerMil});
        if (itA == matrixMapA.end())
        {
            TMatrix matrixA;
            generateRandomSparseMatrix(matrixA, matrixSize, matrixSize, sparsity);
            if (const auto insertIt = matrixMapA.insert({{matrixSize, sparsityPerMil}, matrixA}); insertIt.second)
            {
                a = &insertIt.first->second;
            }
        }
        else
        {
            a = &itA->second;
        }

        auto itB = matrixMapB.find({matrixSize, sparsityPerMil});
        if (itB == matrixMapB.end())
        {
            TMatrix matrixB;
            generateRandomSparseMatrix(matrixB, matrixSize, matrixSize, sparsity);
            if (const auto insertIt = matrixMapB.insert({ {matrixSize, sparsityPerMil}, matrixB}); insertIt.second)
            {
                b = &insertIt.first->second;
            }
        }
        else
        {
            b = &itB->second;
        }

        auto it = productMap.find({matrixSize, sparsityPerMil});
        if (it == productMap.end())
        {
            sofa::linearalgebra::SparseMatrixProduct<TMatrix> p;
            const auto insertIt = productMap.insert({ {matrixSize, sparsityPerMil}, p});
            if (insertIt.second)
            {
                product = &insertIt.first->second;

                product->matrixA = a;
                product->matrixB = b;
                product->computeProduct();
            }
        }
        else
        {
            product = &it->second;
        }

    }

    void regularProduct(benchmark::State& state)
    {
        product->computeRegularProduct();
    }

    void fastProduct(benchmark::State& state)
    {
        product->computeProduct();
    }

    void forceComputingIntersection(benchmark::State& state)
    {
        product->computeProduct(true);
    }

    sofa::linearalgebra::SparseMatrixProduct<TMatrix>* product { nullptr };

private:

    TMatrix* a { nullptr };
    TMatrix* b { nullptr };

    static std::map< std::pair< int, int>, TMatrix> matrixMapA, matrixMapB;
    static std::map< std::pair< int, int>, sofa::linearalgebra::SparseMatrixProduct<TMatrix> > productMap;
};

template<class TMatrix>
std::map< std::pair< int, int>, TMatrix> BM_SparseMatrixProduct<TMatrix>::matrixMapA;
template<class TMatrix>
std::map< std::pair< int, int>, TMatrix> BM_SparseMatrixProduct<TMatrix>::matrixMapB;
template<class TMatrix>
std::map< std::pair< int, int>, sofa::linearalgebra::SparseMatrixProduct<TMatrix> > BM_SparseMatrixProduct<TMatrix>::productMap;

BENCHMARK_TEMPLATE_DEFINE_F(BM_SparseMatrixProduct, RegularProduct, Eigen::SparseMatrix<SReal>)(benchmark::State& st)
{
    for (auto _ : st)
    {
        this->regularProduct(st);
    }
}
BENCHMARK_REGISTER_F(BM_SparseMatrixProduct, RegularProduct)->Unit(benchmark::kMicrosecond)
    ->ArgsProduct({{100, 1000}, {10, 20, 100, 150}})
    ->Args({10000, 1})
    ->Args({10000, 5});

BENCHMARK_TEMPLATE_DEFINE_F(BM_SparseMatrixProduct, FastProduct, Eigen::SparseMatrix<SReal>)(benchmark::State& st)
{
    for (auto _ : st)
    {
        this->fastProduct(st);
    }
}
BENCHMARK_REGISTER_F(BM_SparseMatrixProduct, FastProduct)->Unit(benchmark::kMicrosecond)
    ->ArgsProduct({{100, 1000}, {10, 20, 100, 150}})
    ->Args({10000, 1})
    ->Args({10000, 5});


BENCHMARK_TEMPLATE_DEFINE_F(BM_SparseMatrixProduct, ForceComputingIntersection, Eigen::SparseMatrix<SReal>)(benchmark::State& st)
{
    for (auto _ : st)
    {
        this->forceComputingIntersection(st);
    }
}
// BENCHMARK_REGISTER_F(BM_SparseMatrixProduct, ForceComputingIntersection)->Unit(benchmark::kMicrosecond)
//     ->ArgsProduct({{100, 1000},{10}});
