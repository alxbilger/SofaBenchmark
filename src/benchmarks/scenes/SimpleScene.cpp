#include <benchmarks/scenes/BenchScene.h>

struct SimpleScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<?xml version='1.0'?>
    <Node name='root' gravity='0 0 0' time='0' animate='0'   >
        <DefaultAnimationLoop />
        <MechanicalObject position='0 0 0 4 5 6'/>
        <UniformMass name='m_mass'/>
    </Node>;
    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str(),
            sceneString.size());
    }

    inline static const double dt{ 0.01 };
    inline static const std::size_t nbSteps{ 1000 };
};

constexpr int64_t minSubIterations = 8;
constexpr int64_t maxSubIterations = 128;
constexpr int64_t stepSubIterations = 2;

BENCHMARK_TEMPLATE1(BM_Scene_bench_SimulationFactor, SimpleScene)->RangeMultiplier(stepSubIterations)->Ranges({ {minSubIterations, maxSubIterations} })->Unit(benchmark::kMillisecond);