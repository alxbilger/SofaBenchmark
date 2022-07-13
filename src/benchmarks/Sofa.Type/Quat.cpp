#include <benchmark/benchmark.h>

#include <utils/RandomValuePool.h>

#include <sofa/type/Vec.h>
#include <sofa/type/Quat.h>
#include <array>
#include <numeric>

constexpr int64_t minSubIterations = 8 << 10;
constexpr int64_t maxSubIterations = 8 << 10;

static void BM_Quat_rotateVec(benchmark::State& state);
BENCHMARK(BM_Quat_rotateVec)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
static void BM_Quat_rotateVec2(benchmark::State& state);
BENCHMARK(BM_Quat_rotateVec2)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
static void BM_Quat_rotateVec_unloop(benchmark::State& state);
BENCHMARK(BM_Quat_rotateVec_unloop)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
static void BM_Quat_invrotateVec(benchmark::State& state);
BENCHMARK(BM_Quat_invrotateVec)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

void BM_Quat_rotateVec(benchmark::State& state)
{
    using Quat = sofa::type::Quatf;
    using Vec3 = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 + maxSubIterations * 4;
    const auto& quatValues = RandomValuePool<float, totalsize>::get(0.f, 1.f);
    const auto& vectValues = RandomValuePool<float, totalsize>::get();
    std::vector<Quat> quat;
    std::vector<Vec3> vect;

    for (auto _ : state)
    {
        state.PauseTiming();
        quat.resize(state.range(0));
        vect.resize(state.range(0));

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            quat.emplace_back(Quat{ quatValues[i*4 + 0], quatValues[i * 4 + 1], quatValues[i * 4 + 2], quatValues[i * 4 + 3] });
            vect.emplace_back(Vec3{ vectValues[i*3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2] });
        }
        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(quat[i].rotate(vect[i]));
        }
    }
}

void BM_Quat_rotateVec2(benchmark::State& state)
{
    using Quat = sofa::type::Quatf;
    using Vec3 = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 + maxSubIterations * 4;
    const auto& quatValues = RandomValuePool<float, totalsize>::get(0.f, 1.f);
    const auto& vectValues = RandomValuePool<float, totalsize>::get();
    std::vector<Quat> quat;
    std::vector<Vec3> vect;

    for (auto _ : state)
    {
        state.PauseTiming();
        quat.resize(state.range(0));
        vect.resize(state.range(0));

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            quat.emplace_back(Quat{ quatValues[i * 4 + 0], quatValues[i * 4 + 1], quatValues[i * 4 + 2], quatValues[i * 4 + 3] });
            vect.emplace_back(Vec3{ vectValues[i * 3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2] });
        }
        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(quat[i].rotate(vect[i]));
        }
    }
}

void BM_Quat_rotateVec_unloop(benchmark::State& state)
{
    using Quat = sofa::type::Quatf;
    using Vec3 = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 + maxSubIterations * 4;
    const auto& quatValues = RandomValuePool<float, totalsize>::get(0.f, 1.f);
    const auto& vectValues = RandomValuePool<float, totalsize>::get();

    std::vector<Quat> quat;
    std::vector<Vec3> vect;

    for (auto _ : state)
    {
        state.PauseTiming();
        quat.resize(state.range(0));
        vect.resize(state.range(0));
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            quat.emplace_back(Quat{ quatValues[i * 4 + 0], quatValues[i * 4 + 1], quatValues[i * 4 + 2], quatValues[i * 4 + 3] });
            vect.emplace_back(Vec3{ vectValues[i * 3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2] });
        }
        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            const auto t = Vec3{
                quat[i][1] * vect[i][2] - quat[i][2] * vect[i][1],
                quat[i][2] * vect[i][0] - quat[i][0] * vect[i][2],
                quat[i][0] * vect[i][1] - quat[i][1] * vect[i][0]
            } *2;

            const auto tcross = Vec3{
                quat[i][1] * t[2] - quat[i][2] * t[1],
                quat[i][2] * t[0] - quat[i][0] * t[2],
                quat[i][0] * t[1] - quat[i][1] * t[0]
            };
            const auto res = vect[i] + quat[i][3] * t + tcross;
            benchmark::DoNotOptimize(res);
        }
    }
}

void BM_Quat_invrotateVec(benchmark::State& state)
{
    using Quat = sofa::type::Quatf;
    using Vec3 = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 + maxSubIterations * 4;
    const auto& quatValues = RandomValuePool<float, totalsize>::get(0.f, 1.f);
    const auto& vectValues = RandomValuePool<float, totalsize>::get();
    std::vector<Quat> quat;
    std::vector<Vec3> vect;

    for (auto _ : state)
    {
        state.PauseTiming();
        quat.resize(state.range(0));
        vect.resize(state.range(0));

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            quat.emplace_back(Quat{ quatValues[i*4 + 0], quatValues[i * 4 + 1], quatValues[i * 4 + 2], quatValues[i * 4 + 3] });
            vect.emplace_back(Vec3{ vectValues[i*3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2] });
        }
        state.ResumeTiming();

        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            benchmark::DoNotOptimize(quat[i].inverseRotate(vect[i]));
        }
    }
}

