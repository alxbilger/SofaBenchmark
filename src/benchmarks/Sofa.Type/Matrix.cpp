#include <benchmark/benchmark.h>

#include <utils/RandomValuePool.h>

#include <sofa/type/Mat.h>
#include <Eigen/Core>
#include <Eigen/LU> // for inverse(?)
#include <array>

template<typename ScalarType>
static void BM_Matrix_typemat3x3_construct(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_typemat3x3_construct_noinit(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_typemat3x3_transpose(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_typemat3x3_invert(benchmark::State& state);
template<typename ScalarType, sofa::Size N>
void BM_Matrix_typemat_determinant(benchmark::State& state);
template<typename ScalarType, int N>
static void BM_Matrix_typemat_matmult(benchmark::State& state);
template<typename ScalarType, int N>
static void BM_Matrix_typemat_vecmult(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_typemat3x3_assign(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_typemat3x3_multTranspose(benchmark::State& state);

// eigen version
template<typename ScalarType>
static void BM_Matrix_eigenmat3x3_construct(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_eigenmat3x3_transpose(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_eigenmat3x3_invert(benchmark::State& state);
template<typename ScalarType, sofa::Size N>
void BM_Matrix_eigenmat_determinant(benchmark::State& state);
template<typename ScalarType, int N>
static void BM_Matrix_eigenmat_matmult(benchmark::State& state);
template<typename ScalarType, int N>
static void BM_Matrix_eigenmat_vecmult(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_eigenmat3x3_assign(benchmark::State& state);
template<typename ScalarType>
static void BM_Matrix_eigenmat3x3_multTranspose(benchmark::State& state);

constexpr int64_t minSubIterations = 8 << 6;
constexpr int64_t maxSubIterations = 8 << 8;

#define BMARGS ->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond)

BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_construct, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_construct, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_construct_noinit, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_construct_noinit, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_transpose, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_transpose, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_invert, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_invert, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_determinant, float, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_determinant, double, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_determinant, float, 4) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_determinant, double, 4) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_determinant, float, 12) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_determinant, double, 12) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_matmult, float, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_matmult, double, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_matmult, float, 6) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_matmult, double, 6) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_matmult, float, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_matmult, double, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_vecmult, float, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_vecmult, double, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_vecmult, float, 6) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_vecmult, double, 6) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_vecmult, float, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat_vecmult, double, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_assign, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_assign, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_multTranspose, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typemat3x3_multTranspose, double) BMARGS;

BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_construct, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_construct, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_transpose, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_transpose, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_invert, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_invert, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_determinant, float, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_determinant, double, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_determinant, float, 4) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_determinant, double, 4) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_determinant, float, 12) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_determinant, double, 12) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_matmult, float, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_matmult, double, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_matmult, float, 6) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_matmult, double, 6) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_matmult, float, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_matmult, double, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_vecmult, float, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_vecmult, double, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_vecmult, float, 6) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_vecmult, double, 6) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_vecmult, float, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat_vecmult, double, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_assign, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_assign, double) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_multTranspose, float) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmat3x3_multTranspose, double) BMARGS;

#undef BMARGS

template<typename ScalarType>
void BM_Matrix_typemat3x3_construct(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = sofa::type::Mat<3,3,ScalarType>;
    using Line = typename Matrix::Line;
    
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(Matrix{
                Line{values[i], values[i + 1], values[i + 2]},
                Line{values[i + 3], values[i + 4], values[i + 5]},
                Line{values[i + 6], values[i + 7], values[i + 8] }
            });
        }
    }
}

template<typename ScalarType>
void BM_Matrix_typemat3x3_construct_noinit(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = sofa::type::Mat<3,3,ScalarType>;
    using Line = typename Matrix::LineNoInit;
    
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(Matrix{
                Line{values[i], values[i + 1], values[i + 2]},
                Line{values[i + 3], values[i + 4], values[i + 5]},
                Line{values[i + 6], values[i + 7], values[i + 8] }
            });
        }
    }
}

template<typename ScalarType>
void BM_Matrix_eigenmat3x3_construct(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = Eigen::Matrix<ScalarType, 3, 3>;
    
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            //only eigen3.4
            //Eigen::Matrix<ScalarType, 3,3>(
            //    { vc[i], vc[i + 1], vc[i + 2] },
            //    { vc[i + 3], vc[i + 4], vc[i + 5] },
            //    { vc[i + 6], vc[i + 7], vc[i + 8] }
            //);

            Matrix m;
            m << values[i], values[i + 1], values[i + 2], values[i + 3], values[i + 4], values[i + 5], values[i + 6], values[i + 7], values[i + 8];
            benchmark::DoNotOptimize(m);
        }
    }
}

template<typename ScalarType>
void BM_Matrix_typemat3x3_assign(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = sofa::type::Mat<3,3,ScalarType>;
    using MatrixNoInit = sofa::type::MatNoInit<3,3,ScalarType>;
    using Line = typename Matrix::LineNoInit;
    
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            Matrix m1{
                Line{values[i], values[i + 1], values[i + 2]},
                Line{values[i + 3], values[i + 4], values[i + 5]},
                Line{values[i + 6], values[i + 7], values[i + 8] }
            };
            MatrixNoInit m2{};
            m2 = m1;
            benchmark::DoNotOptimize(m2);
        }
    }
}

template<typename ScalarType>
void BM_Matrix_eigenmat3x3_assign(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = Eigen::Matrix<ScalarType, 3, 3>;
    
    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            Matrix m1;
            m1 << values[i], values[i + 1], values[i + 2], values[i + 3], values[i + 4], values[i + 5], values[i + 6], values[i + 7], values[i + 8];
            Matrix m2;
            m2 = m1;
            benchmark::DoNotOptimize(m2);
        }
    }
}

template<typename ScalarType>
void BM_Matrix_typemat3x3_transpose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = sofa::type::Mat<3,3,ScalarType>;
    using Line = typename Matrix::LineNoInit;
    
    std::vector<Matrix> vc;
    vc.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        vc.emplace_back(
            Line{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
            Line{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
            Line{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
        );
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            benchmark::DoNotOptimize(mat.transposed());
        }
    }
}

template<typename ScalarType>
void BM_Matrix_eigenmat3x3_transpose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = Eigen::Matrix<ScalarType, 3, 3>;
    
    std::vector<Matrix> vc;
    vc.resize(state.range(0));

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        vc[i] << values[i * 9 + 0] , values[i * 9 + 1] , values[i * 9 + 2],
                 values[i * 9 + 3] , values[i * 9 + 4] , values[i * 9 + 5],
                 values[i * 9 + 6] , values[i * 9 + 7] , values[i * 9 + 8];
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            Matrix res = mat.transpose();
            benchmark::DoNotOptimize(res);
        }
    }
}

template<typename ScalarType, int N>
void BM_Matrix_typemat_matmult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * N*N * 2;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = sofa::type::Mat<N,N,ScalarType>;
    using MatrixNoInit = sofa::type::MatNoInit<N,N,ScalarType>;
    
    std::vector<Matrix> vc1;
    std::vector<Matrix> vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        MatrixNoInit mat1, mat2;
        for (int a = 0; a < N; ++a)
        {
            for (int b = 0; b < N; ++b)
            {
                mat1[a][b] = *it++;
                mat2[a][b] = *it++;
            }
        }
        vc1.push_back(mat1);
        vc2.push_back(mat2);
    }

    for (auto _ : state)
    {
        for (auto i = 0 ; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(vc1[i]*vc2[i]);
        }
    }
}

template<typename ScalarType, int N>
void BM_Matrix_eigenmat_matmult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * N*N * 2;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = Eigen::Matrix<ScalarType, N, N>;
    
    std::vector<Matrix> vc1;
    std::vector<Matrix> vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));
    benchmark::DoNotOptimize(vc1);
    benchmark::DoNotOptimize(vc2);

    auto it = values.begin();

    for (auto i = 0; i < state.range(0); i++)
    {
        Matrix mat1, mat2;
        for (int a = 0; a < N; ++a)
        {
            for (int b = 0; b < N; ++b)
            {
                mat1(a,b) = *it++;
                mat2(a,b) = *it++;
            }
        }
        vc1.push_back(mat1);
        vc2.push_back(mat2);
    }

    for (auto _ : state)
    {
        for (auto i = 0; i < state.range(0); i++)
        {
            Matrix res = vc1[i] * vc2[i];
            benchmark::DoNotOptimize(res);
        }
    }
}

template<typename ScalarType, int N>
void BM_Matrix_typemat_vecmult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * (N*N + N);
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = sofa::type::Mat<N,N,ScalarType>;
    using Vec = sofa::type::Vec<N,ScalarType>;
    using MatrixNoInit = sofa::type::MatNoInit<N,N,ScalarType>;
    using VecNoInit = sofa::type::VecNoInit<N,ScalarType>;
    
    std::vector<Matrix> vc1;
    std::vector<Vec> vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        MatrixNoInit mat;
        VecNoInit vec;
        for (int a = 0; a < N; ++a)
        {
            for (int b = 0; b < N; ++b)
            {
                mat[a][b] = *it++;
            }
            vec[a] = *it++;
        }
        vc1.push_back(mat);
        vc2.push_back(vec);
    }

    for (auto _ : state)
    {
        for (auto i = 0 ; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(vc1[i]*vc2[i]);
        }
    }
}

template<typename ScalarType, int N>
void BM_Matrix_eigenmat_vecmult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * (N*N + N);
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = Eigen::Matrix<ScalarType, N, N>;
    using Vec = Eigen::Matrix<ScalarType, N, 1>;
    
    std::vector<Matrix> vc1;
    std::vector<Vec> vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));
    benchmark::DoNotOptimize(vc1);
    benchmark::DoNotOptimize(vc2);

    auto it = values.begin();

    for (auto i = 0; i < state.range(0); i++)
    {
        Matrix mat1;
        Vec mat2;
        for (int a = 0; a < N; ++a)
        {
            for (int b = 0; b < N; ++b)
            {
                mat1(a,b) = *it++;
            }
            mat2(a,0) = *it++;
        }
        vc1.push_back(mat1);
        vc2.push_back(mat2);
    }

    for (auto _ : state)
    {
        for (auto i = 0; i < state.range(0); i++)
        {
            Vec res = vc1[i] * vc2[i];
            benchmark::DoNotOptimize(res);
        }
    }
}

template<typename ScalarType, sofa::Size N>
void BM_Matrix_typemat_determinant(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * N * N;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    std::vector<sofa::type::Mat<N, N, ScalarType>> vc;
    vc.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); ++i)
    {
        sofa::type::Mat<N, N, ScalarType> mat;
        for (sofa::Size r = 0; r < N; ++r)
        {
            for (sofa::Size c = 0; c < N; ++c)
            {
                mat[r][c] = *it++;
            }
        }
        vc.push_back(mat);
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            benchmark::DoNotOptimize(sofa::type::determinant(mat));
        }
    }
}

template<typename ScalarType, sofa::Size N>
void BM_Matrix_eigenmat_determinant(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * N * N;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = Eigen::Matrix<ScalarType, N, N>;
    
    std::vector<Matrix> vc;
    vc.resize(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); ++i)
    {
        Matrix mat;
        for (sofa::Size r = 0; r < N; ++r)
        {
            for (sofa::Size c = 0; c < N; ++c)
            {
                mat(r, c) = *it++;
            }
        }
        vc.push_back(mat);
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            benchmark::DoNotOptimize(mat.determinant());
        }
    }
}

template<typename ScalarType>
void BM_Matrix_typemat3x3_invert(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = sofa::type::Mat<3,3,ScalarType>;
    using Line = typename Matrix::LineNoInit;
    
    std::vector<Matrix> vc;
    vc.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        vc.emplace_back(
            Line{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
            Line{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
            Line{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
        );
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            benchmark::DoNotOptimize(mat.inverted());
        }
    }
}

template<typename ScalarType>
void BM_Matrix_eigenmat3x3_invert(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = Eigen::Matrix<ScalarType, 3, 3>;
    
    std::vector<Matrix> vc;
    vc.resize(state.range(0));

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        vc[i] << values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2],
            values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5],
            values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8];
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            Matrix res = mat.inverse();
            benchmark::DoNotOptimize(res);
        }
    }
}

template<typename ScalarType>
void BM_Matrix_typemat3x3_multTranspose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 3*3 * 2;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = sofa::type::Mat<3,3,ScalarType>;
    using MatrixNoInit = sofa::type::MatNoInit<3,3,ScalarType>;
    
    std::vector<Matrix> vc1;
    std::vector<Matrix> vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        MatrixNoInit mat1, mat2;
        for (int a = 0; a < 3; ++a)
        {
            for (int b = 0; b < 3; ++b)
            {
                mat1[a][b] = *it++;
                mat2[a][b] = *it++;
            }
        }
        vc1.push_back(mat1);
        vc2.push_back(mat2);
    }

    for (auto _ : state)
    {
        for (auto i = 0 ; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(vc1[i].multTranspose(vc2[i]));
        }
    }
}

template<typename ScalarType>
void BM_Matrix_eigenmat3x3_multTranspose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 3*3 * 2;
    const std::array<ScalarType, totalsize>& values = RandomValuePool<ScalarType, totalsize>::get();

    using Matrix = Eigen::Matrix<ScalarType, 3, 3>;
    
    std::vector<Matrix> vc1;
    std::vector<Matrix> vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        Matrix mat1, mat2;
        for (int a = 0; a < 3; ++a)
        {
            for (int b = 0; b < 3; ++b)
            {
                mat1(a, b) = *it++;
                mat2(a, b) = *it++;
            }
        }
        vc1.push_back(mat1);
        vc2.push_back(mat2);
    }

    for (auto _ : state)
    {
        for (auto i = 0 ; i < state.range(0); i++)
        {
            Matrix res = vc1[i].transpose() * vc2[i];
            benchmark::DoNotOptimize(res);
        }
    }
}
