#include "AdvancedTimer.h"

#include <sofa/helper/AdvancedTimer.h>

void BM_AdvancedTimer_begin_end(benchmark::State &state)
{
    for (auto _ : state)
    {
        sofa::helper::AdvancedTimer::begin("Animate");
        sofa::helper::AdvancedTimer::end("Animate");
    }
}
