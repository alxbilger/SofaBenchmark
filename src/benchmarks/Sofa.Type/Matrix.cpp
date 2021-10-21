#include <benchmark/benchmark.h>

#include <utils/RandomValuePool.h>

#include <sofa/type/Mat.h>
#include <Eigen/Core>
#include <Eigen/LU> // for inverse(?)
#include <array>

static void BM_Matrix_typemat3x3f_construct(benchmark::State& state);
static void BM_Matrix_eigenmat33_construct(benchmark::State& state);
static void BM_Matrix_typemat3x3f_transpose(benchmark::State& state);
static void BM_Matrix_eigenmat33_transpose(benchmark::State& state);
static void BM_Matrix_typemat3x3f_invert(benchmark::State& state);
static void BM_Matrix_eigenmat33_invert(benchmark::State& state);
static void BM_Matrix_typemat3x3f_determinant(benchmark::State& state);
static void BM_Matrix_eigenmat33_determinant(benchmark::State& state);
static void BM_Matrix_typemat3x3f_mult(benchmark::State& state);
static void BM_Matrix_eigenmat33_mult(benchmark::State& state);
static void BM_Matrix_typemat3x3f_assign(benchmark::State& state);
static void BM_Matrix_eigenmat33_assign(benchmark::State& state);

constexpr int64_t minSubIterations = 8 << 4;
constexpr int64_t maxSubIterations = 8 << 6;

BENCHMARK(BM_Matrix_typemat3x3f_construct)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_eigenmat33_construct)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_typemat3x3f_transpose)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_eigenmat33_transpose)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_typemat3x3f_invert)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_eigenmat33_invert)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_typemat3x3f_determinant)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_eigenmat33_determinant)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_typemat3x3f_mult)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_eigenmat33_mult)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_typemat3x3f_assign)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_Matrix_eigenmat33_assign)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

void BM_Matrix_typemat3x3f_construct(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(sofa::type::Mat3x3f{ 
                sofa::type::fixed_array<float, 3>{values[i], values[i + 1], values[i + 2]},
                sofa::type::fixed_array<float, 3>{values[i + 3], values[i + 4], values[i + 5]},
                sofa::type::fixed_array<float, 3>{values[i + 6], values[i + 7], values[i + 8] }
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
                sofa::type::fixed_array<float, 3>{values[i], values[i + 1], values[i + 2]},
                sofa::type::fixed_array<float, 3>{values[i + 3], values[i + 4], values[i + 5]},
                sofa::type::fixed_array<float, 3>{values[i + 6], values[i + 7], values[i + 8] }
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

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<sofa::type::Mat3x3f> vc;
        vc.reserve(state.range(0));

        for (unsigned int i = 0; i < state.range(0); i++)
        {
            vc.push_back(sofa::type::Mat3x3f{
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
                });
        }

        state.ResumeTiming();

        for (auto& mat : vc)
        {
            mat.transpose();
            benchmark::DoNotOptimize(mat);
        }
    }
}

void BM_Matrix_eigenmat33_transpose(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<Eigen::Matrix3f> vc;
        vc.resize(state.range(0));

        for (unsigned int i = 0; i < state.range(0); i++)
        {
            vc[i] << values[i * 9 + 0] , values[i * 9 + 1] , values[i * 9 + 2],
                     values[i * 9 + 3] , values[i * 9 + 4] , values[i * 9 + 5],
                     values[i * 9 + 6] , values[i * 9 + 7] , values[i * 9 + 8];
        }

        state.ResumeTiming();

        for (auto& mat : vc)
        {
            mat.transpose();
            benchmark::DoNotOptimize(mat);
        }
    }
}

void BM_Matrix_typemat3x3f_mult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<sofa::type::Mat3x3f> vc1;
        std::vector<sofa::type::Mat3x3f> vc2;
        vc1.reserve(state.range(0));
        vc2.reserve(state.range(0));

        for (unsigned int i = 0; i < state.range(0); i++)
        {
            vc1.push_back(sofa::type::Mat3x3f{
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
                });
            vc2.push_back(sofa::type::Mat3x3f{
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 9], values[i * 9 + 10], values[i * 9 + 11]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 12], values[i * 9 + 13], values[i * 9 + 14]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 15], values[i * 9 + 16], values[i * 9 + 17]}
                });
        }

        state.ResumeTiming();

        for (auto i = 0 ; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(vc1[i]*vc2[i]);
        }
    }
}

void BM_Matrix_eigenmat33_mult(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9 * 2;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<Eigen::Matrix3f> vc1;
        std::vector<Eigen::Matrix3f> vc2;
        vc1.resize(state.range(0));
        vc2.resize(state.range(0));
        benchmark::DoNotOptimize(vc1);
        benchmark::DoNotOptimize(vc2);

        for (auto i = 0; i < state.range(0); i++)
        {
            vc1[i] << values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2],
                      values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5],
                      values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8];
            vc2[i] << values[i * 9 + 9], values[i * 9 + 10], values[i * 9 + 11],
                      values[i * 9 + 12], values[i * 9 + 13], values[i * 9 + 14],
                      values[i * 9 + 15], values[i * 9 + 16], values[i * 9 + 17];
        }
        state.ResumeTiming();

        for (auto i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(vc1[i]* vc2[i]);
        }
    }
}


void BM_Matrix_typemat3x3f_determinant(benchmark::State& state)
{
    constexpr auto totalsize = maxSubIterations * 9;
    const std::array<float, totalsize>& values = RandomValuePool<float, totalsize>::get();

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<sofa::type::Mat3x3f> vc;
        vc.reserve(state.range(0));

        for (unsigned int i = 0; i < state.range(0); i++)
        {
            vc.push_back(sofa::type::Mat3x3f{
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
                });
        }

        state.ResumeTiming();

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

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<Eigen::Matrix3f> vc;
        vc.resize(state.range(0));

        for (unsigned int i = 0; i < state.range(0); i++)
        {
            vc[i] << values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2],
                values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5],
                values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8];
        }

        state.ResumeTiming();

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

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<sofa::type::Mat3x3f> vc;
        vc.reserve(state.range(0));

        for (unsigned int i = 0; i < state.range(0); i++)
        {
            vc.push_back(sofa::type::Mat3x3f{
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5]},
                    sofa::type::fixed_array<float, 3>{values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8]}
                });
        }

        state.ResumeTiming();

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

    for (auto _ : state)
    {
        state.PauseTiming();
        std::vector<Eigen::Matrix3f> vc;
        vc.resize(state.range(0));

        for (unsigned int i = 0; i < state.range(0); i++)
        {
            vc[i] << values[i * 9 + 0], values[i * 9 + 1], values[i * 9 + 2],
                values[i * 9 + 3], values[i * 9 + 4], values[i * 9 + 5],
                values[i * 9 + 6], values[i * 9 + 7], values[i * 9 + 8];
        }

        state.ResumeTiming();

        for (auto& mat : vc)
        {
            benchmark::DoNotOptimize(mat.inverse());
        }
    }
}
