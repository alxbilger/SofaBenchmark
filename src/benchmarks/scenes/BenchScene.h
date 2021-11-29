#pragma once

#include <benchmark/benchmark.h>

#include <sofa/simulation/Simulation.h>
#include <sofa/simulation/Node.h>
#include <SofaSimulationCommon/SceneLoaderXML.h>
#include <SofaSimulationGraph/SimpleApi.h>
#include <SofaSimulationGraph/DAGSimulation.h>

#include <SofaSimulationGraph/init.h>
#include <SofaComponentAll/initSofaComponentAll.h>

using sofa::simulation::setSimulation;

// Generic benchmark for a scene (timing whole animation)
// TScene (template argument) needs to implement getRoot(), dt and nbSteps
template<typename TScene>
void BM_Scene_bench(benchmark::State& state)
{
    sofa::component::initSofaComponentAll();

    sofa::simulation::Simulation* simu = new sofa::simulation::graph::DAGSimulation();
    setSimulation(simu);
    sofa::simulation::Node::SPtr root = TScene::getRoot();

    root->init(sofa::core::execparams::defaultInstance());

    for (auto _ : state)
    {
        state.ResumeTiming();
        for (auto i = 0; i < state.range(0); ++i)
        {
            for (auto j = 0; j < TScene::nbSteps; j++)
            {
                simu->animate(root.get(), TScene::dt);
            }
        }
        state.PauseTiming();
    }

    if (root != nullptr)
    {
        simu->unload(root);
    }
    sofa::simulation::graph::cleanup();
}
