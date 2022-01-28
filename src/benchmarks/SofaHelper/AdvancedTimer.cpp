#include <sstream>
#include <benchmark/benchmark.h>
#include <sofa/helper/AdvancedTimer.h>

///Evaluates the overhead of a call to AdvancedTimer
static void BM_AdvancedTimer_begin_end(benchmark::State& state);
BENCHMARK(BM_AdvancedTimer_begin_end);

static void BM_AdvancedTimer_largeNumberTimers(benchmark::State& state);
BENCHMARK(BM_AdvancedTimer_largeNumberTimers)->Range(8 << 4, 8 << 11)->Unit(benchmark::kMillisecond);

static void BM_AdvancedTimer_deepTreeEnabled(benchmark::State& state);
BENCHMARK(BM_AdvancedTimer_deepTreeEnabled)->ArgsProduct({
    benchmark::CreateRange(1, 1 << 6, 2),
    benchmark::CreateDenseRange(2, 4, 1)
})->Unit(benchmark::kMillisecond);

static void BM_AdvancedTimer_deepTreeDisabled(benchmark::State& state);
BENCHMARK(BM_AdvancedTimer_deepTreeDisabled)->ArgsProduct({
    benchmark::CreateRange(1, 1 << 6, 2),
    benchmark::CreateDenseRange(2, 4, 1)
})->Unit(benchmark::kMillisecond);

void BM_AdvancedTimer_begin_end(benchmark::State &state)
{
    for (auto _ : state)
    {
        sofa::helper::AdvancedTimer::begin("Animate");
        sofa::helper::AdvancedTimer::end("Animate");
    }
}


static const std::vector<std::string> listTimerNames = []()
{
    std::vector<std::string> names;
    std::generate_n(std::back_inserter(names), 1.1 * std::pow(1 << 6, 4), [n = 0]() mutable { return std::to_string(n++);});
    return names;
}();


void BM_AdvancedTimer_largeNumberTimers(benchmark::State& state)
{
    for (auto _ : state)
    {
        sofa::helper::AdvancedTimer::begin("Animate");
        for (int64_t i = 0; i < state.range(0); ++i)
        {
            const auto& timerName = listTimerNames[i];
            sofa::helper::AdvancedTimer::stepBegin(timerName);
            sofa::helper::AdvancedTimer::stepEnd(timerName);
        }
        sofa::helper::AdvancedTimer::end("Animate");
    }
}

void subStep(int64_t depth, int64_t i, int64_t nbTimers, int64_t maxDepth, int64_t& timersCounter)
{
    if (depth == maxDepth)
        return;

    const auto& timerName = listTimerNames[timersCounter];
    sofa::helper::AdvancedTimer::stepBegin(timerName);
    ++timersCounter;

    for (unsigned int j = 0; j < nbTimers; ++j)
    {
        subStep(depth + 1, j, nbTimers, maxDepth, timersCounter);
    }

    sofa::helper::AdvancedTimer::stepEnd(timerName);
}

void advancedTimerDeepTree(benchmark::State& state)
{
    int64_t timersCounter {};
    for (auto _ : state)
    {
        timersCounter = 0;
        sofa::helper::AdvancedTimer::begin("Animate");

        for (int64_t i = 0; i < state.range(0); ++i)
        {
            subStep(0, i, state.range(0), state.range(1), timersCounter);
        }

        sofa::helper::AdvancedTimer::end("Animate");
    }
    state.counters["nbTimers"] = benchmark::Counter( timersCounter);
}

void BM_AdvancedTimer_deepTreeEnabled(benchmark::State& state)
{
    sofa::helper::AdvancedTimer::setEnabled("Animate", true);
    sofa::helper::AdvancedTimer::setInterval("Animate", 1);
    sofa::helper::AdvancedTimer::setOutputType("Animate", "gui");
    advancedTimerDeepTree(state);
}

void BM_AdvancedTimer_deepTreeDisabled(benchmark::State& state)
{
    sofa::helper::AdvancedTimer::setEnabled("Animate", false);
    advancedTimerDeepTree(state);
}
