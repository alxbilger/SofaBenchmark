#include "NarrowPhaseDetection.h"

#include <SofaMeshCollision/PointModel.h>

using sofa::core::objectmodel::New;

void BM_NarrowPhaseDetection_getDetectionOutputs(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        auto narrowPhaseDetection = New<sofa::component::collision::EmptyNarrowPhaseDetection>();

        sofa::helper::vector<sofa::component::collision::PointCollisionModel<sofa::defaulttype::Vec3Types>::SPtr> collisionModels;
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            collisionModels.push_back(New<sofa::component::collision::PointCollisionModel<sofa::defaulttype::Vec3Types> >());
        }

        state.ResumeTiming();
        for (auto a : collisionModels)
        {
            for (auto b : collisionModels)
            {
                narrowPhaseDetection->getDetectionOutputs(a.get(), b.get());
            }
        }
    }
}

void sofa::component::collision::EmptyNarrowPhaseDetection::addCollisionPair(
        const std::pair<core::CollisionModel *, core::CollisionModel *> &cmPair)
{
    SOFA_UNUSED(cmPair);
}
