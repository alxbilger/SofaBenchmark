#include <benchmark/benchmark.h>

#include <SofaBaseLinearSolver/CompressedRowSparseMatrix.inl>
#include <sofa/helper/RandomGenerator.h>

using sofa::component::linearsolver::CompressedRowSparseMatrix;

template<class T>
class BM_CRS_Fixture : public benchmark::Fixture
{
public:
    /// Setup a matrix of size 3000x3000
    void SetUp(const ::benchmark::State& state)
    {
        mat.resize(3 * 1000, 3 * 1000);
    }

    void TearDown(const ::benchmark::State& state)
    {
        mat.clear();
    }

    /// Benchmark where a 3x3 bloc is inserted into 1000 random positions into the matrix
    void addBloc(benchmark::State& state)
    {
        state.PauseTiming();
        mat.clear();
        state.ResumeTiming();

        for (const auto& pos : matrixPositions)
        {
            mat.add(pos.first, pos.second, bloc);
        }
    }

    /// Benchmark where a 3x3 bloc is inserted into 1000 random positions into the matrix
    /// It differs from the addBloc benchmark because the insertion is called with a specialized
    /// function, specific to bloc-based CRS matrices.
    void addBlocShortcut(benchmark::State& state)
    {
        state.PauseTiming();
        mat.clear();
        state.ResumeTiming();

        for (const auto& pos : matrixPositions)
        {
            *(mat.wbloc(pos.first / 3, pos.second / 3, true)) += bloc;
        }
    }

    /// Benchmark where a 3x3 bloc is inserted into 1000 random positions into the matrix
    /// Instead of inserting the 3x3 matrix as a bloc, all the 9 values are inserted invidually
    void addBlocScalar(benchmark::State& state)
    {
        state.PauseTiming();
        mat.clear();
        state.ResumeTiming();

        for (const auto& pos : matrixPositions)
        {
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    mat.add(pos.first + i, pos.second + j, bloc(i,j));
                }
            }

        }
    }

private:
    CompressedRowSparseMatrix<T> mat;

    static const sofa::type::Mat<3, 3, double> bloc;
    static const std::vector< std::pair<sofa::Index, sofa::Index> > matrixPositions;
};
template<class T>
const sofa::type::Mat<3, 3, double> BM_CRS_Fixture<T>::bloc = { {1., 2., 3.}, {4., 5., 6.}, {7., 8., 9.}};
template<class T>
const std::vector< std::pair<sofa::Index, sofa::Index> > BM_CRS_Fixture<T>::matrixPositions = []()
{
    std::vector< std::pair<sofa::Index, sofa::Index> > vec(1000);
    sofa::helper::RandomGenerator randomGenerator;
    randomGenerator.initSeed(12);
    const auto gen = [&randomGenerator]() -> std::pair<sofa::Index, sofa::Index>
    {
        return {3 * randomGenerator.random<sofa::Index>(0, 1000), 3 * randomGenerator.random<sofa::Index>(0, 1000)};
    };
    std::generate(std::begin(vec), std::end(vec), gen);
    return vec;
}();

/// Benchmark the insertion of 1000 3x3 blocs into a CRS made of double
/// Insertion uses the bloc overload
BENCHMARK_TEMPLATE_F(BM_CRS_Fixture, Add3x3Bloc_CRSdouble, double)(benchmark::State& st)
{
    for (auto _ : st)
    {
        this->addBloc(st);
    }
}

/// Benchmark the insertion of 1000 3x3 blocs into a CRS made of 3x3 blocs
/// Insertion uses the bloc overload
BENCHMARK_TEMPLATE_F(BM_CRS_Fixture, Add3x3Bloc_CRS3x3d, sofa::type::Mat<3,3,double>)(benchmark::State& st)
{
    for (auto _ : st)
    {
        this->addBloc(st);
    }
}

/// Benchmark the insertion of 1000 3x3 blocs into a CRS made of 3x3 blocs
/// Insertion uses the bloc insertion specialized for 3x3 CRS matrices
BENCHMARK_TEMPLATE_F(BM_CRS_Fixture, Add3x3BlocShortcut_CRS3x3d, sofa::type::Mat<3,3,double>)(benchmark::State& st)
{
    for (auto _ : st)
    {
        this->addBlocShortcut(st);
    }
}

/// Benchmark the insertion of 1000 3x3 blocs into a CRS made of double
/// Bloc is inserted using 9 individual scalar insertion
BENCHMARK_TEMPLATE_F(BM_CRS_Fixture, Add3x3BlocScalar_double, double)(benchmark::State& st)
{
    for (auto _ : st)
    {
        this->addBlocScalar(st);
    }
}

/// Benchmark the insertion of 1000 3x3 blocs into a CRS made of 3x3 blocs
/// Bloc is inserted using 9 individual scalar insertion
BENCHMARK_TEMPLATE_F(BM_CRS_Fixture, Add3x3BlocScalar_CRS3x3d, sofa::type::Mat<3,3,double>)(benchmark::State& st)
{
    for (auto _ : st)
    {
        this->addBlocScalar(st);
    }
}