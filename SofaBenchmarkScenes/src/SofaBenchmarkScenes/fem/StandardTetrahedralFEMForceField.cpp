#include <SofaBenchmarkScenes/BenchScene.h>

struct StandardTetrahedralFEMForceFieldScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<?xml version="1.0"?>
<Node name="root" dt="0.01" gravity="0 -9 0">
    <RequiredPlugin name="SofaBoundaryCondition"/>
    <RequiredPlugin name="SofaEngine"/>
    <RequiredPlugin name="SofaImplicitOdeSolver"/>
    <RequiredPlugin name="SofaSimpleFem"/>
    <RequiredPlugin name="SofaTopologyMapping"/>

    <VisualStyle displayFlags="showBehaviorModels showForceFields" />
    <DefaultAnimationLoop />

    <Node name="BeamFEM_LARGE">
        <EulerImplicitSolver name="cg_odesolver" printLog="false"  rayleighStiffness="0.1" rayleighMass="0.1" />
        <SparseLDLSolver template="CompressedRowSparseMatrixMat3x3d"/>

        <RegularGridTopology name="grid" min="-5 -5 0" max="5 5 40" n="5 5 20"/>
        <MechanicalObject template="Vec3d"/>

        <TetrahedronSetTopologyContainer name="Tetra_topo"/>
        <TetrahedronSetTopologyModifier name="Modifier" />
        <TetrahedronSetGeometryAlgorithms template="Vec3d" name="GeomAlgo" />
        <Hexa2TetraTopologicalMapping input="@grid" output="@Tetra_topo" />

        <DiagonalMass massDensity="0.2" />
        <StandardTetrahedralFEMForceField name="FEM" ParameterSet="3448.2759 31034.483" materialName="StVenantKirchhoff"/>

        <BoxROI template="Vec3d" name="box_roi" box="-6 -6 -1 6 6 0.1" drawBoxes="1" />
        <FixedConstraint template="Vec3d" indices="@box_roi.indices" />
    </Node>

</Node>

    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str());
    }

    inline static const double dt{ 0.01 };
    inline static const std::size_t nbSteps{ 1000 };
};

// Measure multiple simulations
constexpr int64_t minNbSimulations = 2;
constexpr int64_t maxNbSimulations = 32;
constexpr int64_t stepNbSimulations = 2;

//BENCHMARK_TEMPLATE1(BM_Scene_bench_SimulationFactor, TetrahedronFEMForceFieldScene)->RangeMultiplier(stepNbSimulations)->Ranges({ {minNbSimulations, maxNbSimulations} })->Unit(benchmark::kMillisecond);

// Measure one simulation with increasing number of steps
constexpr int64_t minNbSteps = 32;
constexpr int64_t maxNbSteps = 256;
constexpr int64_t stepNbSteps = 2;

void BM_StandardTetrahedralFEMForceField(benchmark::State& state)
{
    BM_Scene_bench_AdvancedTimer<StandardTetrahedralFEMForceFieldScene>(state, {"MBKBuild", "MBKSolve"});
}

BENCHMARK(BM_StandardTetrahedralFEMForceField)->RangeMultiplier(stepNbSteps)->Ranges({ {minNbSteps, maxNbSteps} })->Unit(benchmark::kMillisecond);
