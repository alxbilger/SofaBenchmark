#pragma once

#include <benchmark/benchmark.h>

#include <sofa/simulation/Simulation.h>
#include <sofa/simulation/Node.h>
#include <SofaSimulationCommon/SceneLoaderXML.h>
#include <SofaSimulationGraph/SimpleApi.h>
#include <SofaSimulationGraph/DAGSimulation.h>

#include <SofaSimulationGraph/init.h>
#include <SofaComponentAll/initSofaComponentAll.h>

#include <boost/intrusive_ptr.hpp>

using sofa::simulation::setSimulation;

// Generic benchmark for a scene (timing whole animation) with a fixed number of steps a certain number of time
// TScene (template argument) needs to implement getRoot(), dt and nbSteps
template<typename TScene>
void BM_Scene_bench_SimulationFactor(benchmark::State& state)
{
    sofa::helper::logging::MessageDispatcher::clearHandlers() ;
    
    sofa::component::initSofaComponentAll();

    sofa::simulation::Simulation* simu = new sofa::simulation::graph::DAGSimulation();
    setSimulation(simu);

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
                simu->animate(root.get(), TScene::dt);
            }
            state.PauseTiming();

            simu->unload(root);
        }
    }

    state.counters["FPS"] = benchmark::Counter(TScene::nbSteps * state.range(0), benchmark::Counter::kIsIterationInvariantRate);
    state.counters["frame"] = benchmark::Counter(TScene::nbSteps * state.range(0), benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);

    sofa::simulation::graph::cleanup();
}

// Generic benchmark for a scene (timing whole animation) with a increasing number of steps at once
// TScene (template argument) needs to implement getRoot() and dt
template<typename TScene>
void BM_Scene_bench_StepFactor(benchmark::State& state)
{
    sofa::component::initSofaComponentAll();

    sofa::simulation::Simulation* simu = new sofa::simulation::graph::DAGSimulation();
    setSimulation(simu);

    for (auto _ : state)
    {
        // Not ideal but did not find a way to clone/duplicate a scene
        sofa::simulation::Node::SPtr root = TScene::getRoot();
        root->init(sofa::core::execparams::defaultInstance());

        state.ResumeTiming();
        for (auto i = 0; i < state.range(0); ++i)
        {
            simu->animate(root.get(), TScene::dt);
        }
        state.PauseTiming();
        simu->unload(root);
    }

    state.counters["FPS"] = benchmark::Counter(state.range(0), benchmark::Counter::kIsIterationInvariantRate);
    state.counters["frame"] = benchmark::Counter(state.range(0), benchmark::Counter::kIsIterationInvariantRate | benchmark::Counter::kInvert);

    sofa::simulation::graph::cleanup();
}

