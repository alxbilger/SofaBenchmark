#include <SofaBenchmarkScenes/BenchScene.h>

struct StiffSpringForceFieldScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<?xml version="1.0"?>
<Node name="root" dt="0.1" gravity="0 -10 0">
    <RequiredPlugin name="SofaBoundaryCondition"/>
    <RequiredPlugin name="SofaDeformable"/>
    <RequiredPlugin name="SofaImplicitOdeSolver"/>

    <DefaultAnimationLoop />
    <EulerImplicitSolver rayleighStiffness="0.1" rayleighMass="0.1"/>
    <CGLinearSolver iterations="3000" tolerance="1e-9" threshold="1e-9"/>

    <Node name="FixedPointNode">
        <MechanicalObject name="dof" position="0 2 0"/>
        <FixedConstraint indices="0"/>
        <SphereCollisionModel radius="0.1"/>
    </Node>

    <Node name="MassNode">
        <MechanicalObject name="dof" position="0 1 0"/>
        <UniformMass name="mass" totalMass="10"/>
        <SphereCollisionModel radius="0.1"/>
    </Node>

    <StiffSpringForceField object1="@FixedPointNode/dof" object2="@MassNode/dof" spring="0 0 100 0 1" drawMode="1"/>
</Node>
    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str(),
            sceneString.size());
    }

    inline static const double dt{ 0.01 };
    inline static const std::size_t nbSteps{ 100 };
};

// Measure multiple simulations
constexpr int64_t minNbSimulations = 2;
constexpr int64_t maxNbSimulations = 32;
constexpr int64_t stepNbSimulations = 2;

BENCHMARK_TEMPLATE1(BM_Scene_bench_SimulationFactor, StiffSpringForceFieldScene)->RangeMultiplier(stepNbSimulations)->Ranges({ {minNbSimulations, maxNbSimulations} })->Unit(benchmark::kMillisecond);

// Measure one simulation with increasing number of steps
constexpr int64_t minNbSteps = 10;
constexpr int64_t maxNbSteps = 1024;
constexpr int64_t stepNbSteps = 2;

BENCHMARK_TEMPLATE1(BM_Scene_bench_StepFactor, StiffSpringForceFieldScene)->RangeMultiplier(stepNbSteps)->Ranges({ {minNbSteps, maxNbSteps} })->Unit(benchmark::kMillisecond);