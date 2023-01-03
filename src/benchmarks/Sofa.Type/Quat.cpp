#include <benchmark/benchmark.h>

#include <utils/RandomValuePool.h>

#include <sofa/type/Vec.h>
#include <sofa/type/Quat.h>

#include <array>
#include <numeric>

constexpr int64_t minSubIterations = 8 << 10;
constexpr int64_t maxSubIterations = 8 << 12;

static void BM_Quat_rotateVec(benchmark::State& state);
BENCHMARK(BM_Quat_rotateVec)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
static void BM_Quat_rotateVec_impl_new(benchmark::State& state);
BENCHMARK(BM_Quat_rotateVec_impl_new)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);
static void BM_Quat_rotateVec_impl_old(benchmark::State& state);
BENCHMARK(BM_Quat_rotateVec_impl_old)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

static void BM_Quat_invrotateVec(benchmark::State& state);
BENCHMARK(BM_Quat_invrotateVec)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

static void BM_Quat_axisToQuat(benchmark::State& state);
BENCHMARK(BM_Quat_axisToQuat)->RangeMultiplier(2)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMicrosecond);

void BM_Quat_rotateVec(benchmark::State& state)
{
    using Quat = sofa::type::Quatf;
    using Vec3 = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 + maxSubIterations * 4;
    const auto& quatValues = RandomValuePool<float, totalsize>::get(0.f, 1.f);
    const auto& vectValues = RandomValuePool<float, totalsize>::get();
    std::vector<Quat> quat;
    std::vector<Vec3> vect;

    quat.reserve(state.range(0));
    vect.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); ++i)
    {
        quat.emplace_back(quatValues[i * 4 + 0], quatValues[i * 4 + 1], quatValues[i * 4 + 2], quatValues[i * 4 + 3]);
        vect.emplace_back(vectValues[i * 3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2]);
    }

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            const auto res = quat[i].rotate(vect[i]);
            benchmark::DoNotOptimize(res);
        }
    }
}


void BM_Quat_rotateVec_impl_new(benchmark::State& state)
{
    using Quat = sofa::type::Quatf;
    using Vec3 = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 + maxSubIterations * 4;
    const auto& quatValues = RandomValuePool<float, totalsize>::get(0.f, 1.f);
    const auto& vectValues = RandomValuePool<float, totalsize>::get();

    std::vector<Quat> quat;
    std::vector<Vec3> vect;

    auto rotateImpl = [](const Quat& q, const Vec3& v)
    {
        const Vec3 qxyz{ q[0], q[1] , q[2] };
        const auto& t = qxyz.cross(v) * 2;
        return (v + q[3] * t + qxyz.cross(t));
    };

    quat.reserve(state.range(0));
    vect.reserve(state.range(0));
    for (unsigned int i = 0; i < state.range(0); ++i)
    {
        quat.emplace_back(quatValues[i * 4 + 0], quatValues[i * 4 + 1], quatValues[i * 4 + 2], quatValues[i * 4 + 3]);
        vect.emplace_back(vectValues[i * 3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2]);
    }

    for (auto _ : state)
    {
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

            const auto res = rotateImpl(quat[i], vect[i]);
            benchmark::DoNotOptimize(res);
        }
    }
}

void BM_Quat_rotateVec_impl_old(benchmark::State& state)
{
    using Quat = sofa::type::Quatf;
    using Vec3 = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3 + maxSubIterations * 4;
    const auto& quatValues = RandomValuePool<float, totalsize>::get(0.f, 1.f);
    const auto& vectValues = RandomValuePool<float, totalsize>::get();
    std::vector<Quat> quat;
    std::vector<Vec3> vect;

    auto previousRotateImpl = [](const Quat& _q, const Vec3& v)
    {
        return Vec3(
            ((1.0f - 2.0f * (_q[1] * _q[1] + _q[2] * _q[2])) * v[0] + (2.0f * (_q[0] * _q[1] - _q[2] * _q[3])) * v[1] + (2.0f * (_q[2] * _q[0] + _q[1] * _q[3])) * v[2]),
            ((2.0f * (_q[0] * _q[1] + _q[2] * _q[3])) * v[0] + (1.0f - 2.0f * (_q[2] * _q[2] + _q[0] * _q[0])) * v[1] + (2.0f * (_q[1] * _q[2] - _q[0] * _q[3])) * v[2]),
            ((2.0f * (_q[2] * _q[0] - _q[1] * _q[3])) * v[0] + (2.0f * (_q[1] * _q[2] + _q[0] * _q[3])) * v[1] + (1.0f - 2.0f * (_q[1] * _q[1] + _q[0] * _q[0])) * v[2])
        );
    };

    quat.reserve(state.range(0));
    vect.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); ++i)
    {
        quat.emplace_back(quatValues[i * 4 + 0], quatValues[i * 4 + 1], quatValues[i * 4 + 2], quatValues[i * 4 + 3]);
        vect.emplace_back(vectValues[i * 3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2]);
    }

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            const auto res = previousRotateImpl(quat[i], vect[i]);
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

    quat.reserve(state.range(0));
    vect.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); ++i)
    {
        quat.emplace_back(quatValues[i * 4 + 0], quatValues[i * 4 + 1], quatValues[i * 4 + 2], quatValues[i * 4 + 3]);
        vect.emplace_back(vectValues[i * 3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2]);
    }

    for (auto _ : state)
    {
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            const auto res = quat[i].inverseRotate(vect[i]);
            benchmark::DoNotOptimize(res);
        }
    }
}

void BM_Quat_axisToQuat(benchmark::State& state)
{
    using Quat = sofa::type::Quatf;
    using Vec3 = sofa::type::Vec3f;

    constexpr auto totalsize = maxSubIterations * 3;
    const auto& vectValues = RandomValuePool<float, totalsize>::get();
    const auto& angles = RandomValuePool<float, maxSubIterations>::get(0.f, 1.f);

    std::vector<Vec3> vect;

    vect.reserve(state.range(0));

    for (unsigned int i = 0; i < state.range(0); ++i)
    {
        vect.emplace_back(vectValues[i * 3 + 0], vectValues[i * 3 + 1], vectValues[i * 3 + 2]);
    }

    for (auto _ : state)
    {
        Quat q(sofa::type::QNOINIT);
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            const auto res = q.axisToQuat(vect[i], angles[i]);
            benchmark::DoNotOptimize(res);
        }
    }
}
