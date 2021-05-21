#include <benchmark/benchmark.h>
#include <sofa/helper/AdvancedTimer.h>

///Evaluates the overhead of a call to AdvancedTimer
static void BM_AdvancedTimer_begin_end(benchmark::State& state);
BENCHMARK(BM_AdvancedTimer_begin_end);

void BM_AdvancedTimer_begin_end(benchmark::State &state)
{
    for (auto _ : state)
    {
        sofa::helper::AdvancedTimer::begin("Animate");
        sofa::helper::AdvancedTimer::end("Animate");
    }
}
