#include <iostream>
#include <benchmark/benchmark.h>

#include <sofa/linearalgebra/SparseMatrixProduct.inl>
#include <sofa/simulation/BaseSimulationExporter.h>
#include <sofa/simulation/MainTaskSchedulerFactory.h>
#include <sofa/simulation/MainTaskSchedulerRegistry.h>
#include <sofa/simulation/ParallelSparseMatrixProduct.h>
#include <utils/SparseMatrix.h>

template<class T>
struct SparseMatrixProductInit
{
    static void init(T& product)
    {
        SOFA_UNUSED(product);
    };

    static void cleanup()
    {
    }
};

template<class Lhs, class Rhs, class ResultType>
struct SparseMatrixProductInit<
    sofa::simulation::ParallelSparseMatrixProduct<Lhs, Rhs, ResultType>>
{
    static void init(sofa::simulation::ParallelSparseMatrixProduct<Lhs, Rhs, ResultType>& product)
    {
        product.taskScheduler = sofa::simulation::MainTaskSchedulerFactory::createInRegistry();
        product.taskScheduler->init();
    };

    static void cleanup()
    {
        sofa::simulation::MainTaskSchedulerRegistry::clear();
    }
};


template<class TProduct>
class BM_SparseMatrixProduct : public benchmark::Fixture
{
public:
    using MatrixLHS = typename TProduct::LhsCleaned;
    using MatrixRHS = typename TProduct::RhsCleaned;

    void SetUp(const ::benchmark::State& state) override
    {
        const auto matrixSize = state.range(0);
        const auto sparsityPerMil = state.range(1);
        const auto sparsity = static_cast<SReal>(sparsityPerMil) / 1000.; //since only integers can be passed as an argument, a number between 0 and 1000 must be provided

        auto itA = matrixMapA.find({matrixSize, sparsityPerMil});
        if (itA == matrixMapA.end())
        {
            MatrixLHS matrixA;
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
            MatrixRHS matrixB;
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
            TProduct p;
            const auto insertIt = productMap.insert({ {matrixSize, sparsityPerMil}, p});
            if (insertIt.second)
            {
                product = &insertIt.first->second;
                SparseMatrixProductInit<TProduct>::init(*product);

                product->m_lhs = a;
                product->m_rhs = b;
                product->computeProduct();
            }
        }
        else
        {
            product = &it->second;
        }

    }

    void TearDown(const ::benchmark::State& state) override
    {
        SparseMatrixProductInit<TProduct>::cleanup();
    }

    void regularProduct(benchmark::State& state)
    {
        SparseMatrixProductInit<TProduct>::init(*product);
        product->computeRegularProduct();
    }

    void fastProduct(benchmark::State& state)
    {
        SparseMatrixProductInit<TProduct>::init(*product);
        product->computeProduct();
    }

    void forceComputingIntersection(benchmark::State& state)
    {
        SparseMatrixProductInit<TProduct>::init(*product);
        product->computeProduct(true);
    }

    TProduct* product { nullptr };

private:

    MatrixLHS* a { nullptr };
    MatrixRHS* b { nullptr };

    static std::map< std::pair< int, int>, MatrixLHS> matrixMapA;
    static std::map< std::pair< int, int>, MatrixRHS> matrixMapB;
    static std::map< std::pair< int, int>, TProduct > productMap;
};

template<class TProduct>
std::map< std::pair< int, int>, typename BM_SparseMatrixProduct<TProduct>::MatrixLHS> BM_SparseMatrixProduct<TProduct>::matrixMapA;
template<class TProduct>
std::map< std::pair< int, int>, typename BM_SparseMatrixProduct<TProduct>::MatrixRHS> BM_SparseMatrixProduct<TProduct>::matrixMapB;
template<class TProduct>
std::map< std::pair< int, int>, TProduct > BM_SparseMatrixProduct<TProduct>::productMap;

#define BENCHARGS \
    ->ArgsProduct({{100, 1000}, {10, 20, 100, 150}}) \
    ->Args({10000, 1}) \
    ->Args({10000, 5}) \

#define SPARSEMATRIXPRODUCTBENCHMARK(Name, ScalarType, ProductType, StorageLHS, StorageRHS, StorageResult) \
    using Product ## Name = ProductType< \
        Eigen::SparseMatrix<ScalarType, StorageLHS>, \
        Eigen::SparseMatrix<ScalarType, StorageRHS>, \
        Eigen::SparseMatrix<ScalarType, StorageResult> \
    >;\
    BENCHMARK_TEMPLATE_DEFINE_F(BM_SparseMatrixProduct, RegularProduct ## Name, Product ## Name)(benchmark::State& st) \
    { \
        for (auto _ : st) \
        {\
            this->regularProduct(st); \
        }\
    } \
    BENCHMARK_TEMPLATE_DEFINE_F(BM_SparseMatrixProduct, FastProduct ## Name, Product ## Name)(benchmark::State& st) \
    { \
        for (auto _ : st) \
        {\
            this->fastProduct(st); \
        }\
    } \
    BENCHMARK_TEMPLATE_DEFINE_F(BM_SparseMatrixProduct, ForceComputingIntersection ## Name, Product ## Name)(benchmark::State& st) \
    { \
        for (auto _ : st) \
        {\
            this->forceComputingIntersection(st); \
        }\
    }\
    BENCHMARK_REGISTER_F(BM_SparseMatrixProduct, RegularProduct ## Name)->Unit(benchmark::kMicrosecond) BENCHARGS;\
    BENCHMARK_REGISTER_F(BM_SparseMatrixProduct, FastProduct ## Name)->Unit(benchmark::kMicrosecond) BENCHARGS;\
    BENCHMARK_REGISTER_F(BM_SparseMatrixProduct, ForceComputingIntersection ## Name)->Unit(benchmark::kMicrosecond) BENCHARGS;


using Eigen::ColMajor;
using Eigen::RowMajor;

SPARSEMATRIXPRODUCTBENCHMARK(ColColCol, SReal, sofa::linearalgebra::SparseMatrixProduct, ColMajor, ColMajor, ColMajor)
SPARSEMATRIXPRODUCTBENCHMARK(RowColCol, SReal, sofa::linearalgebra::SparseMatrixProduct, RowMajor, ColMajor, ColMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ColRowCol, SReal, sofa::linearalgebra::SparseMatrixProduct, ColMajor, RowMajor, ColMajor)
SPARSEMATRIXPRODUCTBENCHMARK(RowRowCol, SReal, sofa::linearalgebra::SparseMatrixProduct, RowMajor, RowMajor, ColMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ColColRow, SReal, sofa::linearalgebra::SparseMatrixProduct, ColMajor, ColMajor, RowMajor)
SPARSEMATRIXPRODUCTBENCHMARK(RowColRow, SReal, sofa::linearalgebra::SparseMatrixProduct, RowMajor, ColMajor, RowMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ColRowRow, SReal, sofa::linearalgebra::SparseMatrixProduct, ColMajor, RowMajor, RowMajor)
SPARSEMATRIXPRODUCTBENCHMARK(RowRowRow, SReal, sofa::linearalgebra::SparseMatrixProduct, RowMajor, RowMajor, RowMajor)

SPARSEMATRIXPRODUCTBENCHMARK(ParColColCol, SReal, sofa::simulation::ParallelSparseMatrixProduct, ColMajor, ColMajor, ColMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ParRowColCol, SReal, sofa::simulation::ParallelSparseMatrixProduct, RowMajor, ColMajor, ColMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ParColRowCol, SReal, sofa::simulation::ParallelSparseMatrixProduct, ColMajor, RowMajor, ColMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ParRowRowCol, SReal, sofa::simulation::ParallelSparseMatrixProduct, RowMajor, RowMajor, ColMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ParColColRow, SReal, sofa::simulation::ParallelSparseMatrixProduct, ColMajor, ColMajor, RowMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ParRowColRow, SReal, sofa::simulation::ParallelSparseMatrixProduct, RowMajor, ColMajor, RowMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ParColRowRow, SReal, sofa::simulation::ParallelSparseMatrixProduct, ColMajor, RowMajor, RowMajor)
SPARSEMATRIXPRODUCTBENCHMARK(ParRowRowRow, SReal, sofa::simulation::ParallelSparseMatrixProduct, RowMajor, RowMajor, RowMajor)

#undef BENCHARGS
#undef SPARSEMATRIXPRODUCTBENCHMARK
