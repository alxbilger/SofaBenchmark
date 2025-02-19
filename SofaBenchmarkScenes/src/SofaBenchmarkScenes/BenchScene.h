#pragma once

#include <benchmark/benchmark.h>

#include <sofa/simulation/Simulation.h>
#include <sofa/simulation/Node.h>
#include <sofa/simulation/common/SceneLoaderXML.h>
#include <sofa/simpleapi/SimpleApi.h>
#include <sofa/simulation/graph/DAGSimulation.h>

#include <sofa/simulation/graph/init.h>
#include <sofa/component/init.h>
#include <sofa/helper/AdvancedTimer.h>

#include <boost/intrusive_ptr.hpp>

using sofa::simulation::setSimulation;

// Generic benchmark for a scene (timing whole animation) with a fixed number of steps a certain number of time
// TScene (template argument) needs to implement getRoot(), dt and nbSteps
template<typename TScene>
void BM_Scene_bench_SimulationFactor(benchmark::State& state)
{
    sofa::helper::logging::MessageDispatcher::clearHandlers() ;
    
    sofa::component::init();

    for (auto _ : state)
    {
        for (auto i = 0; i < state.range(0); ++i)
        {
            state.PauseTiming();

            // Not ideal but did not find a way to clone/duplicate a scene
            sofa::simulation::Node::SPtr root = TScene::getRoot();
            root->init(sofa::core::execparams::defaultInstance());

            state.ResumeTiming();
            for (auto j = 0; j < TScene::nbSteps; j++)
            {
                sofa::simulation::node::animate(root.get(), TScene::dt);
            }

            sofa::simulation::node::unload(root);
        }
    }

    state.counters["FPS"] = benchmark::Counter(TScene::nbSteps * state.range(0), benchmark::Counter::kIsIterationInvariantRate);
    state.counters["frame"] = benchmark::Counter(TScene::nbSteps * state.range(0), benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);

    sofa::simulation::graph::cleanup();
}

inline auto convertInSeconds(sofa::helper::system::thread::ctime_t t)
{
    static auto timer_freqd = static_cast<SReal>(sofa::helper::system::thread::CTime::getTicksPerSec());
    return static_cast<SReal>(t) / static_cast<SReal>(timer_freqd);
};

// Generic benchmark for a scene (timing whole animation), and adding custom counters for specific AdvancedTimer labels
// TScene (template argument) needs to implement getRoot() and dt
template<typename TScene>
void BM_Scene_bench_AdvancedTimer(benchmark::State& state, const std::vector<const char*>& advancedTimerLabels)
{
    sofa::helper::logging::MessageDispatcher::clearHandlers() ;

    sofa::helper::AdvancedTimer::setEnabled("Animate", true);
    sofa::helper::AdvancedTimer::setInterval("Animate", 1);
    sofa::helper::AdvancedTimer::setOutputType("Animate", "gui");

    sofa::component::init();

    sofa::simulation::Simulation* simu = new sofa::simulation::graph::DAGSimulation();

    std::vector<sofa::helper::AdvancedTimer::IdStep> timerIds;
    timerIds.reserve(advancedTimerLabels.size());
    for (const auto& label : advancedTimerLabels)
    {
        timerIds.emplace_back(label);
    }
    std::vector<SReal> avgTimers(advancedTimerLabels.size());

    for (auto _ : state)
    {
        state.PauseTiming();
        // Not ideal but did not find a way to clone/duplicate a scene
        sofa::simulation::Node::SPtr root = TScene::getRoot();
        root->init(sofa::core::execparams::defaultInstance());

        state.ResumeTiming();
        for (auto j = 0; j < state.range(0); j++)
        {
            sofa::helper::AdvancedTimer::begin("Animate");
            sofa::simulation::node::animate(root.get(), TScene::dt);
            sofa::helper::AdvancedTimer::end("Animate");

            const auto records = sofa::helper::AdvancedTimer::getStepData("Animate", true);
            for (unsigned int i = 0; i < advancedTimerLabels.size(); ++i)
            {
                const auto findIt = records.find(timerIds[i]);
                if (findIt == records.end())
                {
                    state.SkipWithError(("Timer " + std::string(advancedTimerLabels[i]) + " not found in this simulation").c_str());
                    break;
                }
                const auto& stepData = findIt->second;
                const auto duration = convertInSeconds(stepData.ttotal / stepData.num);
                avgTimers[i] += duration;
            }
        }

        sofa::helper::AdvancedTimer::clearData("Animate");

        sofa::simulation::node::unload(root);
    }

    std::transform(avgTimers.begin(), avgTimers.end(), avgTimers.begin(), [&state](SReal t) { return t / state.range(0);});
    for (unsigned int i = 0; i < advancedTimerLabels.size(); ++i)
    {
        state.counters[advancedTimerLabels[i]] = benchmark::Counter(avgTimers[i], benchmark::Counter::kAvgIterations);
    }
    state.counters["FPS"] = benchmark::Counter(state.range(0), benchmark::Counter::kIsIterationInvariantRate); // nbTimeSteps * nbIterations / totalDuration
    state.counters["frame"] = benchmark::Counter(state.range(0), benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);

    sofa::simulation::graph::cleanup();
}

// Generic benchmark for a scene (timing whole animation) with a increasing number of steps at once
// TScene (template argument) needs to implement getRoot() and dt
template<typename TScene>
void BM_Scene_bench_StepFactor(benchmark::State& state)
{
    sofa::helper::logging::MessageDispatcher::clearHandlers() ;

    sofa::component::init();

    sofa::simulation::Simulation* simu = new sofa::simulation::graph::DAGSimulation();

    for (auto _ : state)
    {
        state.PauseTiming();

        // Not ideal but did not find a way to clone/duplicate a scene
        sofa::simulation::Node::SPtr root = TScene::getRoot();
        root->init(sofa::core::execparams::defaultInstance());

        state.ResumeTiming();
        for (auto i = 0; i < state.range(0); ++i)
        {
            sofa::simulation::node::animate(root.get(), TScene::dt);
        }

        sofa::simulation::node::unload(root);
    }

    state.counters["FPS"] = benchmark::Counter(state.range(0), benchmark::Counter::kIsIterationInvariantRate);
    state.counters["frame"] = benchmark::Counter(state.range(0), benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);

    sofa::simulation::graph::cleanup();
}

