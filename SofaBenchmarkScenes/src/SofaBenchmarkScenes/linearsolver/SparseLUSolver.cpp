#include <SofaBenchmarkScenes/BenchScene.h>

struct SparseLUSolverScene
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

        <SparseLUSolver template="CompressedRowSparseMatrixd"/>
        <HexahedronFEMForceField name="FEM" youngModulus="4000" poissonRatio="0.3" method="large" />
    </Node>
</Node>

    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str());
    }

    inline static const double dt{ 0.02 };
    inline static const std::size_t nbSteps{ 100 };
};

struct SparseLUSolverSceneMat3x3
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

        <SparseLUSolver template="CompressedRowSparseMatrixMat3x3d"/>
        <HexahedronFEMForceField name="FEM" youngModulus="4000" poissonRatio="0.3" method="large" />
    </Node>
</Node>

    )SCENE_DELIM";

        return sofa::simulation::SceneLoaderXML::loadFromMemory("scene_xml",
            sceneString.c_str());
    }

    inline static const double dt{ 0.02 };
    inline static const std::size_t nbSteps{ 100 };
};


void BM_SparseLUSolver(benchmark::State& state)
{
    BM_Scene_bench_AdvancedTimer<SparseLUSolverScene>(state, {"MBKBuild", "MBKSolve"});
}

BENCHMARK(BM_SparseLUSolver)->Arg(50)->Unit(benchmark::kMillisecond)->Iterations(10);

void BM_SparseLUSolverMat3x3(benchmark::State& state)
{
    BM_Scene_bench_AdvancedTimer<SparseLUSolverSceneMat3x3>(state, {"MBKBuild", "MBKSolve"});
}

BENCHMARK(BM_SparseLUSolverMat3x3)->Arg(50)->Unit(benchmark::kMillisecond)->Iterations(10);
