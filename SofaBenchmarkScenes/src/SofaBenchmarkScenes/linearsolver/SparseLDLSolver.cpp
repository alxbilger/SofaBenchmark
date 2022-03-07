#include <SofaBenchmarkScenes/BenchScene.h>

struct SparseLDLSolverScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<Node name="root" dt="0.02" gravity="0 -10 0">
    <RequiredPlugin name="SofaBoundaryCondition"/>
    <RequiredPlugin name="SofaImplicitOdeSolver"/>
    <RequiredPlugin name="SofaSimpleFem"/>
    <RequiredPlugin name="SofaSparseSolver"/>

    <VisualStyle displayFlags="showBehaviorModels showForceFields" />

    <DefaultAnimationLoop/>
    <DefaultVisualManagerLoop/>

    <Node name="M1">
        <EulerImplicitSolver name="odesolver" rayleighStiffness="0.1" rayleighMass="0.1" />
        <MechanicalObject name="DoFs" />
        <UniformMass name="mass" totalMass="320" />
        <RegularGridTopology name="grid" nx="4" ny="4" nz="20" xmin="-9" xmax="-6" ymin="0" ymax="3" zmin="0" zmax="19" />
        <BoxROI name="box" box="-10 -1 -0.0001  -5 4 0.0001"/>
        <FixedConstraint indices="@box.indices" />

        <SparseLDLSolver template="CompressedRowSparseMatrixMat3x3d"/>
        <HexahedronFEMForceField name="FEM" youngModulus="4000" poissonRatio="0.3" method="large" />
    </Node>
</Node>

    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str(),
            sceneString.size());
    }

    inline static const double dt{ 0.02 };
    inline static const std::size_t nbSteps{ 100 };
};

// Measure multiple simulations
constexpr int64_t minNbSimulations = 2;
constexpr int64_t maxNbSimulations = 16;
constexpr int64_t stepNbSimulations = 2;

BENCHMARK_TEMPLATE1(BM_Scene_bench_SimulationFactor, SparseLDLSolverScene)->RangeMultiplier(stepNbSimulations)->Ranges({ {minNbSimulations, maxNbSimulations} })->Unit(benchmark::kMillisecond);

// Measure one simulation with increasing number of steps
constexpr int64_t minNbSteps = 512;
constexpr int64_t maxNbSteps = 2048;
constexpr int64_t stepNbSteps = 2;

BENCHMARK_TEMPLATE1(BM_Scene_bench_StepFactor, SparseLDLSolverScene)->RangeMultiplier(stepNbSteps)->Ranges({ {minNbSteps, maxNbSteps} })->Unit(benchmark::kMillisecond);
