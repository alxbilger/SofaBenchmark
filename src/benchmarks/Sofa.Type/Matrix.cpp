#include <benchmark/benchmark.h>

#include <utils/RandomValuePool.h>

#include <sofa/type/Mat.h>
#include <Eigen/Core>
#include <Eigen/LU> // for inverse(?)
#include <array>

static void BM_Matrix_typemat3x3f_construct(benchmark::State& state);
static void BM_Matrix_typemat3x3f_construct_noinit(benchmark::State& state);
static void BM_Matrix_eigenmat33_construct(benchmark::State& state);
static void BM_Matrix_typemat3x3f_transpose(benchmark::State& state);
static void BM_Matrix_eigenmat33_transpose(benchmark::State& state);
static void BM_Matrix_typemat3x3f_invert(benchmark::State& state);
static void BM_Matrix_eigenmat33_invert(benchmark::State& state);
static void BM_Matrix_typemat3x3f_determinant(benchmark::State& state);
static void BM_Matrix_eigenmat33_determinant(benchmark::State& state);
template<int N>
static void BM_Matrix_typematf_matmult(benchmark::State& state);
template<int N>
static void BM_Matrix_eigenmatf_matmult(benchmark::State& state);
template<int N>
static void BM_Matrix_typematf_vecmult(benchmark::State& state);
template<int N>
static void BM_Matrix_eigenmatf_vecmult(benchmark::State& state);
static void BM_Matrix_typemat3x3f_assign(benchmark::State& state);
static void BM_Matrix_eigenmat33_assign(benchmark::State& state);
static void BM_Matrix_typemat3x3f_multTranspose(benchmark::State& state);
static void BM_Matrix_eigenmat3x3f_multTranspose(benchmark::State& state);


static void BM_Matrix_typemat3x3f_noconstexpr_bench(benchmark::State& state);
static void BM_Matrix_typemat3x3f_constexpr_bench(benchmark::State& state);

constexpr int64_t minSubIterations = 8 << 4;
constexpr int64_t maxSubIterations = 8 << 6;

#define BMARGS ->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond)

BENCHMARK(BM_Matrix_typemat3x3f_construct) BMARGS;
BENCHMARK(BM_Matrix_typemat3x3f_construct_noinit) BMARGS;
BENCHMARK(BM_Matrix_eigenmat33_construct) BMARGS;
BENCHMARK(BM_Matrix_typemat3x3f_transpose) BMARGS;
BENCHMARK(BM_Matrix_eigenmat33_transpose) BMARGS;
BENCHMARK(BM_Matrix_typemat3x3f_invert) BMARGS;
BENCHMARK(BM_Matrix_eigenmat33_invert) BMARGS;
BENCHMARK(BM_Matrix_typemat3x3f_determinant) BMARGS;
BENCHMARK(BM_Matrix_eigenmat33_determinant) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typematf_matmult, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typematf_matmult, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmatf_matmult, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmatf_matmult, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typematf_vecmult, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_typematf_vecmult, 24) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmatf_vecmult, 3) BMARGS;
BENCHMARK_TEMPLATE(BM_Matrix_eigenmatf_vecmult, 24) BMARGS;
BENCHMARK(BM_Matrix_typemat3x3f_assign) BMARGS;
BENCHMARK(BM_Matrix_eigenmat33_assign) BMARGS;
BENCHMARK(BM_Matrix_typemat3x3f_multTranspose) BMARGS;
BENCHMARK(BM_Matrix_eigenmat3x3f_multTranspose) BMARGS;

#undef BMARGS

BENCHMARK(BM_Matrix_typemat3x3f_noconstexpr_bench)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_typemat3x3f_constexpr_bench)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

void BM_Matrix_typemat3x3f_construct(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(sofa::type::Mat3x3f{
                sofa::type::Vec<3, float>{values[i], values[i + 1], values[i + 2]},
                sofa::type::Vec<3, float>{values[i + 3], values[i + 4], values[i + 5]},
                sofa::type::Vec<3, float>{values[i + 6], values[i + 7], values[i + 8] }
            });
        }
    }
}

void BM_Matrix_typemat3x3f_construct_noinit(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(sofa::type::Mat3x3f{
                sofa::type::Mat3x3f::LineNoInit{values[i], values[i + 1], values[i + 2]},
                sofa::type::Mat3x3f::LineNoInit{values[i + 3], values[i + 4], values[i + 5]},
                sofa::type::Mat3x3f::LineNoInit{values[i + 6], values[i + 7], values[i + 8] }
            });
        }
    }
}

void BM_Matrix_eigenmat33_construct(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            //only eigen3.4
            //Eigen::Matrix<float, 3,3>(
            //    { vc[i], vc[i + 1], vc[i + 2] },
            //    { vc[i + 3], vc[i + 4], vc[i + 5] },
            //    { vc[i + 6], vc[i + 7], vc[i + 8] }
            //);

            Eigen::Matrix<float, 3, 3> m;
            m << values[i], values[i + 1], values[i + 2], values[i + 3], values[i + 4], values[i + 5], values[i + 6], values[i + 7], values[i + 8];
            benchmark::DoNotOptimize(m);
        }
    }
}

void BM_Matrix_typemat3x3f_assign(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            sofa::type::Mat3x3f m1{
                sofa::type::Mat3x3f::LineNoInit{values[i], values[i + 1], values[i + 2]},
                sofa::type::Mat3x3f::LineNoInit{values[i + 3], values[i + 4], values[i + 5]},
                sofa::type::Mat3x3f::LineNoInit{values[i + 6], values[i + 7], values[i + 8] }
            };
            sofa::type::Mat3x3f m2;
            m2 = m1;
            benchmark::DoNotOptimize(m2);
        }
    }
}

void BM_Matrix_eigenmat33_assign(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            Eigen::Matrix<float, 3, 3> m1;
            m1 << values[i], values[i + 1], values[i + 2], values[i + 3], values[i + 4], values[i + 5], values[i + 6], values[i + 7], values[i + 8];
            Eigen::Matrix<float, 3, 3> m2;
            m2 = m1;
            benchmark::DoNotOptimize(m2);
        }
    }
}

void BM_Matrix_typemat3x3f_transpose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<sofa::type::Mat3x3f> vc;
    vc.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        vc.push_back(sofa::type::Mat3x3f{
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
            });
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            auto t = mat.transposed();
            benchmark::DoNotOptimize(t);
        }
    }
}

void BM_Matrix_eigenmat33_transpose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<Eigen::Matrix3f> vc;
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
            Eigen::Matrix3f t = mat.transpose();
            benchmark::DoNotOptimize(t);
        }
    }
}

template<int N>
void BM_Matrix_typematf_matmult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * N*N * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<sofa::type::Mat<N, N, float > > vc1;
    std::vector<sofa::type::Mat<N, N, float > > vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        sofa::type::Mat<N, N, float > mat1, mat2;
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

template<int N>
void BM_Matrix_eigenmatf_matmult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * N*N * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<Eigen::Matrix<float, N, N > > vc1;
    std::vector<Eigen::Matrix<float, N, N > > vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));
    benchmark::DoNotOptimize(vc1);
    benchmark::DoNotOptimize(vc2);

    auto it = values.begin();

    for (auto i = 0; i < state.range(0); i++)
    {
        Eigen::Matrix<float, N, N > mat1, mat2;
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
            Eigen::Matrix<float, N, N > product = vc1[i] * vc2[i];
            benchmark::DoNotOptimize(product);
        }
    }
}

template <int N>
void BM_Matrix_typematf_vecmult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * (N*N + N);
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<sofa::type::Mat<N, N, float > > vc1;
    std::vector<sofa::type::Vec<N, float > > vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        sofa::type::Mat<N, N, float > mat;
        sofa::type::Vec<N, float > vec;
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

template <int N>
void BM_Matrix_eigenmatf_vecmult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * (N*N + N);
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<Eigen::Matrix<float, N, N > > vc1;
    std::vector<Eigen::Matrix<float, N, 1 > > vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));
    benchmark::DoNotOptimize(vc1);
    benchmark::DoNotOptimize(vc2);

    auto it = values.begin();

    for (auto i = 0; i < state.range(0); i++)
    {
        Eigen::Matrix<float, N, N > mat1;
        Eigen::Matrix<float, N, 1 > mat2;
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
            Eigen::Matrix<float, N, 1 > product = vc1[i] * vc2[i];
            benchmark::DoNotOptimize(product);
        }
    }
}


void BM_Matrix_typemat3x3f_determinant(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<sofa::type::Mat3x3f> vc;
    vc.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        vc.push_back(sofa::type::Mat3x3f{
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
            });
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            benchmark::DoNotOptimize(sofa::type::determinant(mat));
        }
    }
}

void BM_Matrix_eigenmat33_determinant(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<Eigen::Matrix3f> vc;
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
            benchmark::DoNotOptimize(mat.determinant());
        }
    }
}


void BM_Matrix_typemat3x3f_invert(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<sofa::type::Mat3x3f> vc;
    vc.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        vc.push_back(sofa::type::Mat3x3f{
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
                sofa::type::Mat3x3f::LineNoInit{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
            });
    }

    for (auto _ : state)
    {
        for (auto& mat : vc)
        {
            benchmark::DoNotOptimize(mat.inverted());
        }
    }
}

void BM_Matrix_eigenmat33_invert(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<Eigen::Matrix3f> vc;
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
            Eigen::Matrix3f inv = mat.inverse();
            benchmark::DoNotOptimize(inv);
        }
    }
}

void BM_Matrix_typemat3x3f_multTranspose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 3*3 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<sofa::type::Mat<3, 3, float > > vc1;
    std::vector<sofa::type::Mat<3, 3, float > > vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        sofa::type::Mat<3, 3, float > mat1, mat2;
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

void BM_Matrix_eigenmat3x3f_multTranspose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 3*3 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    std::vector<Eigen::Matrix<float, 3, 3> > vc1;
    std::vector<Eigen::Matrix<float, 3, 3> > vc2;
    vc1.reserve(state.range(0));
    vc2.reserve(state.range(0));

    auto it = values.begin();

    for (unsigned int i = 0; i < state.range(0); i++)
    {
        Eigen::Matrix<float, 3, 3> mat1, mat2;
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
            Eigen::Matrix<float, 3, 3 > product = vc1[i].transpose() * vc2[i];
            benchmark::DoNotOptimize(product);
        }
    }
}

void BM_Matrix_typemat3x3f_constexpr_bench(benchmark::State& state)
{
    constexpr auto totalsize = 9;
    constexpr auto values = CompileTimeRandomValuePool<float, totalsize>::get();

    constexpr sofa::type::Mat3x3f mat{
                sofa::type::Mat3x3f::LineNoInit{values[0], values[1], values[2]},
                sofa::type::Mat3x3f::LineNoInit{values[3], values[4], values[5]},
                sofa::type::Mat3x3f::LineNoInit{values[6], values[7], values[8]}
    };

    for (auto _ : state)
    {
        constexpr sofa::type::Mat3x3f res = mat.transposed() * mat.transposed() * mat.transposed() / 3.0f;
        benchmark::DoNotOptimize(res);
    }
}

void BM_Matrix_typemat3x3f_noconstexpr_bench(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const auto values = RandomValuePool<float, totalsize>::get();

    const sofa::type::Mat3x3f mat{
                sofa::type::Mat3x3f::LineNoInit{values[0], values[1], values[2]},
                sofa::type::Mat3x3f::LineNoInit{values[3], values[4], values[5]},
                sofa::type::Mat3x3f::LineNoInit{values[6], values[7], values[8]}
    };

    for (auto _ : state)
    {
        const sofa::type::Mat3x3f res = mat.transposed() * mat.transposed() * mat.transposed() / 3.0f;
        benchmark::DoNotOptimize(res);
    }
}
