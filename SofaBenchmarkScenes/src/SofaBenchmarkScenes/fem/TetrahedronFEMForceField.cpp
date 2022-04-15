#include <SofaBenchmarkScenes/BenchScene.h>

struct TetrahedronFEMForceFieldScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<?xml version="1.0"?>
<Node name="root" dt="0.01" gravity="0 -9 0">
	<RequiredPlugin name="Sofa.Component.Constraint.Projective"/> <!-- Needed to use components [FixedConstraint] -->
	<RequiredPlugin name="Sofa.Component.LinearSolver.Iterative"/> <!-- Needed to use components [CGLinearSolver] -->
	<RequiredPlugin name="Sofa.Component.Mass"/> <!-- Needed to use components [DiagonalMass] -->
	<RequiredPlugin name="Sofa.Component.ODESolver.Backward"/> <!-- Needed to use components [EulerImplicitSolver] -->
	<RequiredPlugin name="Sofa.Component.SolidMechanics.FEM.Elastic"/> <!-- Needed to use components [TetrahedronFEMForceField] -->
	<RequiredPlugin name="Sofa.Component.StateContainer"/> <!-- Needed to use components [MechanicalObject] -->
	<RequiredPlugin name="Sofa.Component.Topology.Container.Dynamic"/> <!-- Needed to use components [TetrahedronSetGeometryAlgorithms, TetrahedronSetTopologyContainer, TetrahedronSetTopologyModifier] -->
	<RequiredPlugin name="Sofa.Component.Topology.Container.Grid"/> <!-- Needed to use components [RegularGridTopology] -->
	<RequiredPlugin name="Sofa.Component.Topology.Mapping"/> <!-- Needed to use components [Hexa2TetraTopologicalMapping] -->
	<RequiredPlugin name="Sofa.Component.Visual"/> <!-- Needed to use components [VisualStyle] -->
	<RequiredPlugin name="SofaEngine"/> <!-- Needed to use components [BoxROI] -->

    <VisualStyle displayFlags="showBehaviorModels hideForceFields" />
    <DefaultAnimationLoop />

    <Node name="Beam_TetrahedronFEMForceField">
        <EulerImplicitSolver name="cg_odesolver" printLog="false"  rayleighStiffness="0.1" rayleighMass="0.1" />
        <CGLinearSolver iterations="25" name="linear solver" tolerance="1.0e-9" threshold="1.0e-9" />

        <RegularGridTopology name="grid" min="-5 -5 0" max="5 5 40" n="5 5 20"/>
        <MechanicalObject template="Vec3d" translation="11 0 0"/>

        <TetrahedronSetTopologyContainer name="Tetra_topo"/>
        <TetrahedronSetTopologyModifier name="Modifier" />
        <TetrahedronSetGeometryAlgorithms template="Vec3d" name="GeomAlgo" />
        <Hexa2TetraTopologicalMapping input="@grid" output="@Tetra_topo" />

        <DiagonalMass massDensity="0.2" />
        <TetrahedronFEMForceField name="FEM" youngModulus="1000" poissonRatio="0.4" computeGlobalMatrix="false"
        method="large" computeVonMisesStress="0" showVonMisesStressPerElement="0"/>

        <BoxROI template="Vec3d" name="box_roi" box="-6 -6 -1 50 6 0.1" drawBoxes="1" />
        <FixedConstraint template="Vec3d" indices="@box_roi.indices" />
    </Node>
</Node>

    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str(),
            sceneString.size());
    }

    inline static const double dt{ 0.01 };
    inline static const std::size_t nbSteps{ 1000 };
};


struct TetrahedralFEMForceFieldScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<?xml version="1.0"?>
<Node name="root" dt="0.01" gravity="0 -9 0">
	<RequiredPlugin name="Sofa.Component.Constraint.Projective"/> <!-- Needed to use components [FixedConstraint] -->
	<RequiredPlugin name="Sofa.Component.LinearSolver.Iterative"/> <!-- Needed to use components [CGLinearSolver] -->
	<RequiredPlugin name="Sofa.Component.Mass"/> <!-- Needed to use components [DiagonalMass] -->
	<RequiredPlugin name="Sofa.Component.ODESolver.Backward"/> <!-- Needed to use components [EulerImplicitSolver] -->
	<RequiredPlugin name="Sofa.Component.SolidMechanics.FEM.Elastic"/> <!-- Needed to use components [TetrahedralCorotationalFEMForceField] -->
	<RequiredPlugin name="Sofa.Component.StateContainer"/> <!-- Needed to use components [MechanicalObject] -->
	<RequiredPlugin name="Sofa.Component.Topology.Container.Dynamic"/> <!-- Needed to use components [TetrahedronSetGeometryAlgorithms, TetrahedronSetTopologyContainer, TetrahedronSetTopologyModifier] -->
	<RequiredPlugin name="Sofa.Component.Topology.Container.Grid"/> <!-- Needed to use components [RegularGridTopology] -->
	<RequiredPlugin name="Sofa.Component.Topology.Mapping"/> <!-- Needed to use components [Hexa2TetraTopologicalMapping] -->
	<RequiredPlugin name="Sofa.Component.Visual"/> <!-- Needed to use components [VisualStyle] -->
	<RequiredPlugin name="SofaEngine"/> <!-- Needed to use components [BoxROI] -->

    <VisualStyle displayFlags="showBehaviorModels hideForceFields" />
    <DefaultAnimationLoop />

    <Node name="Beam_TetrahedronFEMForceField">
        <EulerImplicitSolver name="cg_odesolver" printLog="false"  rayleighStiffness="0.1" rayleighMass="0.1" />
        <CGLinearSolver iterations="25" name="linear solver" tolerance="1.0e-9" threshold="1.0e-9" />

        <RegularGridTopology name="grid" min="-5 -5 0" max="5 5 40" n="5 5 20"/>
        <MechanicalObject template="Vec3d" translation="11 0 0"/>

        <TetrahedronSetTopologyContainer name="Tetra_topo"/>
        <TetrahedronSetTopologyModifier name="Modifier" />
        <TetrahedronSetGeometryAlgorithms template="Vec3d" name="GeomAlgo" />
        <Hexa2TetraTopologicalMapping input="@grid" output="@Tetra_topo" />

        <DiagonalMass massDensity="0.2" />
        <TetrahedralCorotationalFEMForceField name="CFEM" youngModulus="1000" poissonRatio="0.4" method="large" />

        <BoxROI template="Vec3d" name="box_roi" box="-6 -6 -1 50 6 0.1" drawBoxes="1" />
        <FixedConstraint template="Vec3d" indices="@box_roi.indices" />
    </Node>
</Node>

    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str(),
            sceneString.size());
    }

    inline static const double dt{ 0.01 };
    inline static const std::size_t nbSteps{ 1000 };
};


struct TetrahedralFEMForceFieldOptimScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<?xml version="1.0"?>
<Node name="root" dt="0.01" gravity="0 -9 0">
	<RequiredPlugin name="Sofa.Component.Constraint.Projective"/> <!-- Needed to use components [FixedConstraint] -->
	<RequiredPlugin name="Sofa.Component.LinearSolver.Iterative"/> <!-- Needed to use components [CGLinearSolver] -->
	<RequiredPlugin name="Sofa.Component.Mass"/> <!-- Needed to use components [DiagonalMass] -->
	<RequiredPlugin name="Sofa.Component.ODESolver.Backward"/> <!-- Needed to use components [EulerImplicitSolver] -->
    <RequiredPlugin name="Sofa.Component.SolidMechanics.FEM.Elastic"/> <!-- Needed to use components [FastTetrahedralCorotationalForceField] -->
	<RequiredPlugin name="Sofa.Component.StateContainer"/> <!-- Needed to use components [MechanicalObject] -->
	<RequiredPlugin name="Sofa.Component.Topology.Container.Dynamic"/> <!-- Needed to use components [TetrahedronSetGeometryAlgorithms, TetrahedronSetTopologyContainer, TetrahedronSetTopologyModifier] -->
	<RequiredPlugin name="Sofa.Component.Topology.Container.Grid"/> <!-- Needed to use components [RegularGridTopology] -->
	<RequiredPlugin name="Sofa.Component.Topology.Mapping"/> <!-- Needed to use components [Hexa2TetraTopologicalMapping] -->
	<RequiredPlugin name="Sofa.Component.Visual"/> <!-- Needed to use components [VisualStyle] -->
	<RequiredPlugin name="SofaEngine"/> <!-- Needed to use components [BoxROI] -->

    <VisualStyle displayFlags="showBehaviorModels hideForceFields" />
    <DefaultAnimationLoop />

    <Node name="Beam_TetrahedronFEMForceField">
        <EulerImplicitSolver name="cg_odesolver" printLog="false"  rayleighStiffness="0.1" rayleighMass="0.1" />
        <CGLinearSolver iterations="25" name="linear solver" tolerance="1.0e-9" threshold="1.0e-9" />

        <RegularGridTopology name="grid" min="-5 -5 0" max="5 5 40" n="5 5 20"/>
        <MechanicalObject template="Vec3d" translation="11 0 0"/>

        <TetrahedronSetTopologyContainer name="Tetra_topo"/>
        <TetrahedronSetTopologyModifier name="Modifier" />
        <TetrahedronSetGeometryAlgorithms template="Vec3d" name="GeomAlgo" />
        <Hexa2TetraTopologicalMapping input="@grid" output="@Tetra_topo" />

        <DiagonalMass massDensity="0.2" />
        <FastTetrahedralCorotationalForceField name="CFEM" youngModulus="1000" poissonRatio="0.4" method="large" />

        <BoxROI template="Vec3d" name="box_roi" box="-6 -6 -1 50 6 0.1" drawBoxes="1" />
        <FixedConstraint template="Vec3d" indices="@box_roi.indices" />
    </Node>
</Node>

    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str(),
            sceneString.size());
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
constexpr int64_t minNbSteps = 512;
constexpr int64_t maxNbSteps = 4096;
constexpr int64_t stepNbSteps = 2;

BENCHMARK_TEMPLATE1(BM_Scene_bench_StepFactor, TetrahedronFEMForceFieldScene)->RangeMultiplier(stepNbSteps)->Ranges({ {minNbSteps, maxNbSteps} })->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE1(BM_Scene_bench_StepFactor, TetrahedralFEMForceFieldScene)->RangeMultiplier(stepNbSteps)->Ranges({ {minNbSteps, maxNbSteps} })->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE1(BM_Scene_bench_StepFactor, TetrahedralFEMForceFieldOptimScene)->RangeMultiplier(stepNbSteps)->Ranges({ {minNbSteps, maxNbSteps} })->Unit(benchmark::kMillisecond);
