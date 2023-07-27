#include <benchmark/benchmark.h>
#include <sofa/config.h>
#include <sofa/core/behavior/BaseLocalForceFieldMatrix.h>
#include <sofa/core/behavior/ForceField.h>
#include <sofa/simulation/Simulation.h>
#include <sofa/simulation/graph/SimpleApi.h>
#include <sofa/simulation/Node.h>
#include <sofa/simulation/graph/DAGNode.h>

/**
 * Benchmark of TetrahedronFEMForceField::buildStiffnessMatrix
 *
 * A cantilever beam.
 * Number of elements depends on the benchmark parameter
 * The scene is initialized and one time step is performed
 * After that, the function buildStiffnessMatrix is called.
 * The function does not accumulate anything because it uses StiffnessMatrixAccumulator which does nothing.
 * However, the calls to virtual functions is still taken into account in the benchmark.
 */
static void BM_TetrahedronFEMForceField_buildStiffnessMatrix(benchmark::State& state)
{
    const auto multiplier = state.range(0);
    const sofa::simulation::NodeSPtr root = sofa::core::objectmodel::New<sofa::simulation::graph::DAGNode>();
    sofa::simpleapi::createObject(root, "DefaultAnimationLoop");

    sofa::simpleapi::importPlugin("Sofa.Component.ODESolver.Backward");
    sofa::simpleapi::createObject(root, "EulerImplicitSolver", {{"rayleighStiffness", "0.1"}, {"rayleighMass","0.1"}});

    sofa::simpleapi::importPlugin("Sofa.Component.LinearSolver.Direct");
    sofa::simpleapi::createObject(root, "EigenSimplicialLLT", {{"template", "CompressedRowSparseMatrixMat3x3"}});

    sofa::simpleapi::importPlugin("Sofa.Component.Topology.Container.Grid");
    sofa::simpleapi::createObject(root, "RegularGridTopology",
                                  {
                                      {"name", "grid"},
                                      {"nx", std::to_string(2 * multiplier)}, {"ny", std::to_string(2 * multiplier)}, {"nz", std::to_string(10 * multiplier)},
                                      {"xmin", "-9"}, {"xmax", "-6"}, {"ymin", "0"}, {"ymax", "3"}, {"zmin", "0"}, {"zmax", "19"}
                                  });

    sofa::simpleapi::importPlugin("Sofa.Component.StateContainer");
    sofa::simpleapi::createObject(root, "MechanicalObject");

    sofa::simpleapi::importPlugin("Sofa.Component.Topology.Container.Dynamic");
    sofa::simpleapi::createObject(root, "TetrahedronSetTopologyContainer", {{"name", "topo"}});
    sofa::simpleapi::createObject(root, "TetrahedronSetTopologyModifier");
    sofa::simpleapi::createObject(root, "TetrahedronSetGeometryAlgorithms");

    sofa::simpleapi::importPlugin("Sofa.Component.Topology.Mapping");
    sofa::simpleapi::createObject(root, "Hexa2TetraTopologicalMapping", { {"input", "@grid"}, {"output", "@topo"}});

    sofa::simpleapi::importPlugin("Sofa.Component.Mass");
    sofa::simpleapi::createObject(root, "UniformMass", { {"totalMass", "320"}});

    sofa::simpleapi::importPlugin("Sofa.Component.Engine.Select");
    sofa::simpleapi::createObject(root, "BoxROI", {{"name", "box"}, { "box", "-10 -1 -0.0001  -5 4 0.0001"}});

    sofa::simpleapi::importPlugin("Sofa.Component.Constraint.Projective");
    sofa::simpleapi::createObject(root, "FixedConstraint", { {"indices", "@box.indices"}});

    sofa::simpleapi::importPlugin("Sofa.Component.SolidMechanics.FEM.Elastic");
    const auto forceFieldObject = sofa::simpleapi::createObject(root, "TetrahedronFEMForceField", { {"youngModulus", "4000"}, {"poissonRatio", "0.3"}, { "method", "large"}});

    sofa::simulation::node::initRoot(root.get());

    sofa::core::behavior::ForceField<sofa::defaulttype::Vec3dTypes>::SPtr forcefield = dynamic_cast<sofa::core::behavior::ForceField<sofa::defaulttype::Vec3dTypes>*>(forceFieldObject.get());

    if (forcefield)
    {
        sofa::core::behavior::StiffnessMatrixAccumulator acc;

        sofa::core::behavior::StiffnessMatrix matrix;
        matrix.setMatrixAccumulator(&acc, forcefield->getMState(), forcefield->getMState());

        sofa::simulation::node::animate(root.get(), 0.1_sreal);

        for (auto _ : state)
        {
            forcefield->buildStiffnessMatrix(&matrix);
        }
    }
}


BENCHMARK(BM_TetrahedronFEMForceField_buildStiffnessMatrix)
->RangeMultiplier(2)->Ranges({ {1, 4} })->Unit(benchmark::kMicrosecond);
