#include <SofaBenchmarkScenes/BenchScene.h>

struct DiagonalMassScene
{
    static auto getRoot()
    {
        const std::string sceneString = R"SCENE_DELIM(
<?xml version="1.0" ?>
<Node name="root" dt="0.005">
    <DefaultAnimationLoop />
    <DefaultPipeline verbose="0" name="CollisionPipeline" />
    <BruteForceBroadPhase/>
    <BVHNarrowPhase/>
    <DefaultContactManager response="PenalityContactForceField" name="collision response" />
    <DiscreteIntersection />

    <MeshGmshLoader name="loader" filename="mesh/liver.msh" />
    <MeshObjLoader name="meshLoader_0" filename="mesh/liver-smooth.obj" handleSeams="1" />

    <Node name="Liver" depend="topo dofs">
        <EulerImplicitSolver name="integration scheme" />
        <CGLinearSolver name="linear solver" iterations="1000" tolerance="1e-9" threshold="1e-9"/>
        <MechanicalObject name="dofs" src="@../loader" />
        <!-- Container for the tetrahedra-->
        <TetrahedronSetTopologyContainer name="TetraTopo" src="@../loader" />
        <TetrahedronSetGeometryAlgorithms name="GeomAlgo" />
        <DiagonalMass totalMass="60" name="diagonalMass" />
        <TetrahedralCorotationalFEMForceField template="Vec3d" name="FEM" method="large" poissonRatio="0.45" youngModulus="5000" />
        <FixedConstraint name="FixedConstraint" indices="3 39 64" />
        
        <Node name="Visu">
            <OglModel name="VisualModel" src="@../../meshLoader_0" color="red" />
            <BarycentricMapping name="VisualMapping" input="@../dofs" output="@VisualModel" />
        </Node>
        <Node name="Surf">
    	    <SphereLoader filename="mesh/liver.sph" />
            <MechanicalObject name="spheres" position="@[-1].position" />
            <SphereCollisionModel name="CollisionModel" listRadius="@[-2].listRadius" />
            <BarycentricMapping name="CollisionMapping" input="@../dofs" output="@spheres" />
        </Node>
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

BENCHMARK_TEMPLATE1(BM_Scene_bench_SimulationFactor, DiagonalMassScene)->RangeMultiplier(stepNbSimulations)->Ranges({ {minNbSimulations, maxNbSimulations} })->Unit(benchmark::kMillisecond);

// Measure one simulation with increasing number of steps
constexpr int64_t minNbSteps = 512;
constexpr int64_t maxNbSteps = 4096;
constexpr int64_t stepNbSteps = 2;

BENCHMARK_TEMPLATE1(BM_Scene_bench_StepFactor, DiagonalMassScene)->RangeMultiplier(stepNbSteps)->Ranges({ {minNbSteps, maxNbSteps} })->Unit(benchmark::kMillisecond);
