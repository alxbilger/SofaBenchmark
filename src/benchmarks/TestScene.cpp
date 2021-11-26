#include <benchmark/benchmark.h>

#include <sofa/simulation/Simulation.h>
#include <sofa/simulation/Node.h>
#include <SofaSimulationCommon/SceneLoaderXML.h>
#include <SofaSimulationGraph/SimpleApi.h>
#include <SofaSimulationGraph/DAGSimulation.h>

#include <SofaSimulationGraph/init.h>
#include <SofaComponentAll/initSofaComponentAll.h>

using sofa::simulation::setSimulation;

void BM_Scene_bench(benchmark::State& state)
{
    const std::string sceneString = R"SCENE_DELIM(
<?xml version='1.0'?>
    <Node name='root' gravity='0 0 0' time='0' animate='0'   >
        <DefaultAnimationLoop />
        <MechanicalObject position='0 0 0 4 5 6'/>
        <UniformMass name='m_mass'/>
    </Node>;
    )SCENE_DELIM";

    const double SCENE_DT = 0.01;
    const std::size_t SCENE_NUMBER_OF_STEPS = 1000;

    sofa::component::initSofaComponentAll();

    sofa::simulation::Simulation* m_simu{ nullptr };
    sofa::simulation::Node::SPtr m_root;
    m_simu = new sofa::simulation::graph::DAGSimulation();
    setSimulation(m_simu);
    m_root = m_simu->createNewGraph("root");

    m_root = sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
        sceneString.c_str(),
        sceneString.size());

    m_root->init(sofa::core::execparams::defaultInstance());

    for (auto _ : state)
    {
        state.ResumeTiming();
        for (auto i = 0; i < state.range(0); ++i)
        {
            for (auto j = 0; j < SCENE_NUMBER_OF_STEPS; j++)
            {
                m_simu->animate(m_root.get(), SCENE_DT);
            }
        }
        state.PauseTiming();
    }

    if (m_root != nullptr)
    {
        m_simu->unload(m_root);
    }
    sofa::simulation::graph::cleanup();
}

constexpr int64_t minSubIterations = 8;
constexpr int64_t maxSubIterations = 128;
constexpr int64_t stepSubIterations = 2;
BENCHMARK(BM_Scene_bench)->RangeMultiplier(stepSubIterations)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMillisecond);