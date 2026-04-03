#include "NarrowPhaseDetection.h"

#include <sofa/component/collision/geometry/PointCollisionModel.h>
#include <benchmark/benchmark.h>

using sofa::core::objectmodel::New;

/// Measure the time take by NarrowPhaseDetection::getDetectionOutputs.
/// It depends on the type of associative array used.
static void BM_NarrowPhaseDetection_getDetectionOutputs(benchmark::State& state);
BENCHMARK(BM_NarrowPhaseDetection_getDetectionOutputs)->RangeMultiplier(2)->Ranges({{8, 8 << 5}, {1, 2}})->Unit(benchmark::kMicrosecond);

static void BM_NarrowPhaseDetection_endNarrowPhase(benchmark::State& state);
BENCHMARK(BM_NarrowPhaseDetection_endNarrowPhase)->RangeMultiplier(2)->Range(8, 8 << 5)->Unit(benchmark::kMicrosecond);

void BM_NarrowPhaseDetection_getDetectionOutputs(benchmark::State &state)
{
    sofa::type::vector<sofa::component::collision::geometry::PointCollisionModel<sofa::defaulttype::Vec3Types>::SPtr> collisionModels;
    collisionModels.reserve(state.range(0));
    std::generate_n(std::back_inserter(collisionModels), state.range(0), [](){ return New<sofa::component::collision::geometry::PointCollisionModel<sofa::defaulttype::Vec3Types> >();});

    for (auto _ : state)
    {
        state.PauseTiming();
        auto narrowPhaseDetection = New<sofa::component::collision::EmptyNarrowPhaseDetection>();
        state.ResumeTiming();

        for (int i = 0; i < state.range(1); ++i)
        {
            for (auto a : collisionModels)
            {
                for (auto b : collisionModels)
                {
                    narrowPhaseDetection->getDetectionOutputs(a.get(), b.get());
                }
            }
        }
    }
}

void BM_NarrowPhaseDetection_endNarrowPhase(benchmark::State &state)
{
    sofa::type::vector<sofa::component::collision::geometry::PointCollisionModel<sofa::defaulttype::Vec3Types>::SPtr> collisionModels;
    collisionModels.reserve(state.range(0));
    std::generate_n(std::back_inserter(collisionModels), state.range(0), [](){ return New<sofa::component::collision::geometry::PointCollisionModel<sofa::defaulttype::Vec3Types> >();});

    for (auto _ : state)
    {
        state.PauseTiming();

        auto narrowPhaseDetection = New<sofa::component::collision::EmptyNarrowPhaseDetection>();
        for (auto a : collisionModels)
        {
            for (auto b : collisionModels)
            {
                narrowPhaseDetection->getDetectionOutputs(a.get(), b.get());
            }
        }

        state.ResumeTiming();
        narrowPhaseDetection->endNarrowPhase();
    }
}

void sofa::component::collision::EmptyNarrowPhaseDetection::addCollisionPair(
        const std::pair<core::CollisionModel *, core::CollisionModel *> &cmPair)
{
    SOFA_UNUSED(cmPair);
}
