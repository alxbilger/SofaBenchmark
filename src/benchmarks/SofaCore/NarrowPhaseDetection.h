#pragma once

#include <sofa/core/collision/NarrowPhaseDetection.h>

namespace sofa::component::collision
{
    /// Empty class inheriting from the abstract class NarrowPhaseDetection
    class EmptyNarrowPhaseDetection : public sofa::core::collision::NarrowPhaseDetection
    {
    public:
        SOFA_ABSTRACT_CLASS(EmptyNarrowPhaseDetection, sofa::core::collision::NarrowPhaseDetection);

        void addCollisionPair(const std::pair<core::CollisionModel *, core::CollisionModel *> &cmPair) override;
    };
}