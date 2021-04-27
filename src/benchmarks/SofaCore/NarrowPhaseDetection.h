#pragma once

#include <benchmark/benchmark.h>

#include <sofa/core/collision/NarrowPhaseDetection.h>

/// Measure the time take by NarrowPhaseDetection::getDetectionOutputs.
/// It depends on the type of associative array used.
static void BM_NarrowPhaseDetection_getDetectionOutputs(benchmark::State& state);
BENCHMARK(BM_NarrowPhaseDetection_getDetectionOutputs)->RangeMultiplier(2)->Range(8, 8 << 5)->Unit(benchmark::kMicrosecond);

namespace sofa::component::collision
{
    /// Empty class inheriting from the abstract class NarrowPhaseDetection
    class EmptyNarrowPhaseDetection : public sofa::core::collision::NarrowPhaseDetection
    {
    public:
        void addCollisionPair(const std::pair<core::CollisionModel *, core::CollisionModel *> &cmPair) override;
    };
}