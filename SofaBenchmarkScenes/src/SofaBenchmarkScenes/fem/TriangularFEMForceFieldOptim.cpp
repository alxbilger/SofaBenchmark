#include <SofaBenchmarkScenes/BenchScene.h>

struct TriangularFEMForceFieldOptimScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<Node name="root" dt="0.05" gravity="0 10 10" showBoundingTree="0">
    <RequiredPlugin name="Sofa.Component.IO.Mesh"/> <!-- Needed to use components [MeshGmshLoader] -->
    <RequiredPlugin name="Sofa.Component.LinearSolver.Direct"/> <!-- Needed to use components [SparseLDLSolver] -->
    <RequiredPlugin name="Sofa.Component.Mapping.Linear"/> <!-- Needed to use components [IdentityMapping] -->
    <RequiredPlugin name="Sofa.Component.Mass"/> <!-- Needed to use components [DiagonalMass] -->
    <RequiredPlugin name="Sofa.Component.ODESolver.Backward"/> <!-- Needed to use components [EulerImplicitSolver] -->
    <RequiredPlugin name="Sofa.Component.SolidMechanics.FEM.Elastic"/> <!-- Needed to use components [TriangularFEMForceFieldOptim] -->
    <RequiredPlugin name="Sofa.Component.StateContainer"/> <!-- Needed to use components [MechanicalObject] -->
    <RequiredPlugin name="Sofa.Component.Visual"/> <!-- Needed to use components [VisualStyle] -->
    <RequiredPlugin name="Sofa.GL.Component.Rendering3D"/> <!-- Needed to use components [OglModel] -->
    <RequiredPlugin name="SofaBoundaryCondition"/> <!-- Needed to use components [FixedConstraint] -->

    <DefaultAnimationLoop />

    <MeshGmshLoader filename="mesh/square3.msh" name="loaderSquare" />
    <Node name="SquareGravity1">
        <EulerImplicitSolver name="odesolver1" rayleighStiffness="0.1" rayleighMass="0.1" />
        <SparseLDLSolver template="CompressedRowSparseMatrixMat3x3d"/>
        <TriangleSetTopologyContainer name="Container" src="@../loaderSquare" />
        <MechanicalObject name="DOFs" src="@../loaderSquare" scale="100"  />
        <TriangleSetTopologyModifier name="Modifier" />
        <TriangleSetGeometryAlgorithms name="GeomAlgo" template="Vec3d" />
        <DiagonalMass massDensity="0.005" />
        <FixedConstraint indices="0 1 2" />
        <TriangularFEMForceFieldOptim name="FEM" youngModulus="600" poissonRatio="0.3" method="large" printLog="1"/>
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

// Measure one simulation with increasing number of steps
constexpr int64_t minNbSteps = 32;
constexpr int64_t maxNbSteps = 256;
constexpr int64_t stepNbSteps = 2;

void BM_TriangularFEMForceFieldOptim(benchmark::State& state)
{
    BM_Scene_bench_AdvancedTimer<TriangularFEMForceFieldOptimScene>(state, {"MBKBuild", "MBKSolve"});
}

BENCHMARK(BM_TriangularFEMForceFieldOptim)->RangeMultiplier(stepNbSteps)->Ranges({ {minNbSteps, maxNbSteps} })->Unit(benchmark::kMillisecond);
