import os
os.environ['BENCHMARK_OUT'] = 'linear_solvers_benchmark.csv'
os.environ['BENCHMARK_OUT_FORMAT'] = 'csv'

import google_benchmark as benchmark
import Sofa
import SofaRuntime
from SofaRuntime import Timer
import Sofa.Gui
from Sofa.Core import TaskScheduler


# Import all components from the SOFA core
SofaRuntime.importPlugin("Sofa.Component")
# Import the MultiThreading plugin
hasMultiThreading = SofaRuntime.importPlugin("MultiThreading")
# Import the MultiThreading plugin
hasCudaLinearSolvers = SofaRuntime.importPlugin("SofaCUDALinearSolver")

with_gui = False


def linear_solver_beam(root, scale, add_main_solver, add_second_solver=None, fem_type='HexahedronFEMForceField'):
    root.addObject('DefaultAnimationLoop')
    root.addObject('VisualStyle', displayFlags="showBehaviorModels showForceFields")

    root.addObject('EulerImplicitSolver', rayleighStiffness="0.1", rayleighMass="0.1")

    add_main_solver(root)
    if add_second_solver:
        add_second_solver(root)

    root.addObject('MechanicalObject', name="DoFs")
    root.addObject('UniformMass', name="mass", totalMass="320")
    root.addObject('RegularGridTopology', name="grid", nx=2 * scale, ny=2 * scale, nz=10 * scale, xmin="-9", xmax="-6", ymin="0", ymax="3", zmin="0", zmax="19")
    root.addObject('BoxROI', name="box", box="-10 -1 -0.0001  -5 4 0.0001")
    root.addObject('FixedConstraint', indices="@box.indices")
    root.addObject(fem_type, name="FEM", youngModulus="4000", poissonRatio="0.45", method="large")


def linear_solver_beam_benchmark(state, add_main_solver, add_second_solver=None, fem_type='HexahedronFEMForceField'):

    while state:
        root = Sofa.Core.Node("root")
        linear_solver_beam(root, state.range(0), add_main_solver, add_second_solver, fem_type)
        Sofa.Simulation.init(root)

        if with_gui:
            Sofa.Gui.GUIManager.Init("myscene", "imgui")
            Sofa.Gui.GUIManager.createGUI(root, __file__)
            Sofa.Gui.GUIManager.SetDimension(1080, 1080)
            # Initialization of the scene will be done here
            Sofa.Gui.GUIManager.MainLoop(root)
            Sofa.Gui.GUIManager.closeGUI()

        Timer.setEnabled("Animate", True)
        Timer.setOutputType("Animate", "json")  # hack to avoid printing in the console

        state.counters["nbElements"] = len(root.grid.hexahedra.value)

        nb_time_steps = 100
        avg_solver_duration = 0

        for iteration in range(nb_time_steps):
            Timer.begin("Animate")
            Sofa.Simulation.animate(root, root.dt.value)

            # The first iteration is not added in the measures because precomputations often happen during this iteration
            if iteration != 0:
                records = Timer.getRecords("Animate")
                # print(records)
                solver_duration = records['Simulation::animate']['solve']['Mechanical (root)']['MBKSolve']['total_time']
                avg_solver_duration += solver_duration

            Timer.end("Animate")

        Timer.clear()
        Sofa.Simulation.unload(root)
        if hasMultiThreading:
            TaskScheduler.stop()

        avg_solver_duration = avg_solver_duration / (nb_time_steps - 1)
        state.set_iteration_time(avg_solver_duration / 1000.)


minScaleFactor = 2
maxScaleFactor = 5
nbIterations = 1


@benchmark.register
@benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
@benchmark.option.unit(benchmark.kMillisecond)
@benchmark.option.use_manual_time()
@benchmark.option.iterations(nbIterations)
def beam_simulation_SparseLDLSolver(state):
    def add_main_solver(root):
        root.addObject('SparseLDLSolver', name='main_solver', applyPermutation=False, template="CompressedRowSparseMatrixMat3x3d")
    linear_solver_beam_benchmark(state, add_main_solver)


@benchmark.register
@benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
@benchmark.option.unit(benchmark.kMillisecond)
@benchmark.option.use_manual_time()
@benchmark.option.iterations(nbIterations)
def beam_simulation_SparseLDLSolver_permuted(state):
    def add_main_solver(root):
        root.addObject('SparseLDLSolver', name='main_solver', applyPermutation=True, template="CompressedRowSparseMatrixMat3x3d")
    linear_solver_beam_benchmark(state, add_main_solver)


@benchmark.register
@benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
@benchmark.option.unit(benchmark.kMillisecond)
@benchmark.option.use_manual_time()
@benchmark.option.iterations(nbIterations)
def beam_simulation_EigenSimplicialLDLT(state):
    def add_main_solver(root):
        root.addObject('EigenSimplicialLDLT', name='main_solver', ordering="Natural", template="CompressedRowSparseMatrixMat3x3d")
    linear_solver_beam_benchmark(state, add_main_solver)


@benchmark.register
@benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
@benchmark.option.unit(benchmark.kMillisecond)
@benchmark.option.use_manual_time()
@benchmark.option.iterations(nbIterations)
def beam_simulation_CGLinearSolver_assembled(state):
    def add_main_solver(root):
        root.addObject('CGLinearSolver', name='main_solver', iterations=1000, tolerance=1e-9, threshold=1e-9, template="CompressedRowSparseMatrixMat3x3d")
    linear_solver_beam_benchmark(state, add_main_solver)


@benchmark.register
@benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
@benchmark.option.unit(benchmark.kMillisecond)
@benchmark.option.use_manual_time()
@benchmark.option.iterations(nbIterations)
def beam_simulation_CGLinearSolver_matrixFree(state):
    def add_main_solver(root):
        root.addObject('CGLinearSolver', name='main_solver', iterations=1000, tolerance=1e-9, threshold=1e-9, template="GraphScattered")
    linear_solver_beam_benchmark(state, add_main_solver)


if hasMultiThreading:
    @benchmark.register
    @benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
    @benchmark.option.unit(benchmark.kMillisecond)
    @benchmark.option.use_manual_time()
    @benchmark.option.iterations(nbIterations)
    def beam_simulation_ParallelCGLinearSolver_assembled(state):
        def add_main_solver(root):
            root.addObject('ParallelCGLinearSolver', name='main_solver', iterations=1000, tolerance=1e-9, threshold=1e-9,
                           template="ParallelCompressedRowSparseMatrixMat3x3d")

        linear_solver_beam_benchmark(state, add_main_solver)


    @benchmark.register
    @benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
    @benchmark.option.unit(benchmark.kMillisecond)
    @benchmark.option.use_manual_time()
    @benchmark.option.iterations(nbIterations)
    def beam_simulation_ParallelCGLinearSolver_matrixFree(state):
        def add_main_solver(root):
            root.addObject('CGLinearSolver', name='main_solver', iterations=1000, tolerance=1e-9, threshold=1e-9,
                           template="GraphScattered")

        linear_solver_beam_benchmark(state, add_main_solver, add_second_solver=None, fem_type='ParallelHexahedronFEMForceField')


@benchmark.register
@benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
@benchmark.option.unit(benchmark.kMillisecond)
@benchmark.option.use_manual_time()
@benchmark.option.iterations(nbIterations)
def beam_simulation_PCG_SparseLDL(state):

    def add_main_solver(root):
        root.addObject('ShewchukPCGLinearSolver', name='main_solver', iterations=1000, template="GraphScattered", preconditioner='@preconditioner')

    def add_second_solver(root):
        root.addObject('SparseLDLSolver', name='preconditioner', applyPermutation=False, template="CompressedRowSparseMatrixMat3x3d")

    linear_solver_beam_benchmark(state, add_main_solver, add_second_solver)


@benchmark.register
@benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
@benchmark.option.unit(benchmark.kMillisecond)
@benchmark.option.use_manual_time()
@benchmark.option.iterations(nbIterations)
def beam_simulation_PCG_AsyncSparseLDL(state):

    def add_main_solver(root):
        root.addObject('ShewchukPCGLinearSolver', name='main_solver', iterations=1000, template="GraphScattered", preconditioner='@preconditioner')

    def add_second_solver(root):
        root.addObject('AsyncSparseLDLSolver', name='preconditioner', applyPermutation=False, template="CompressedRowSparseMatrixMat3x3d")

    linear_solver_beam_benchmark(state, add_main_solver, add_second_solver)


if hasMultiThreading:
    @benchmark.register
    @benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
    @benchmark.option.unit(benchmark.kMillisecond)
    @benchmark.option.use_manual_time()
    @benchmark.option.iterations(nbIterations)
    def beam_simulation_ParallelPCG_AsyncSparseLDL(state):
        def add_main_solver(root):
            root.addObject('ShewchukPCGLinearSolver', name='main_solver', iterations=1000, template="GraphScattered", preconditioner='@preconditioner')

        def add_second_solver(root):
            root.addObject('AsyncSparseLDLSolver', name='preconditioner', applyPermutation=False, template="CompressedRowSparseMatrixMat3x3d")

        linear_solver_beam_benchmark(state, add_main_solver, add_second_solver, fem_type='ParallelHexahedronFEMForceField')


if hasCudaLinearSolvers:
    @benchmark.register
    @benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
    @benchmark.option.unit(benchmark.kMillisecond)
    @benchmark.option.use_manual_time()
    @benchmark.option.iterations(nbIterations)
    def beam_simulation_CUDACholeskySolver(state):
        def add_main_solver(root):
            root.addObject('CUDASparseCholeskySolver', name='main_solver', permutation='None',
                           template="CompressedRowSparseMatrixMat3x3d")

        linear_solver_beam_benchmark(state, add_main_solver)


    @benchmark.register
    @benchmark.option.dense_range(minScaleFactor, maxScaleFactor, 1)
    @benchmark.option.unit(benchmark.kMillisecond)
    @benchmark.option.use_manual_time()
    @benchmark.option.iterations(nbIterations)
    def beam_simulation_CUDACholeskySolver_permuted(state):
        def add_main_solver(root):
            root.addObject('CUDASparseCholeskySolver', name='main_solver', permutation='METIS',
                           template="CompressedRowSparseMatrixMat3x3d")

        linear_solver_beam_benchmark(state, add_main_solver)


# Class used to fake a benchmark, so it can run without the Google Benchmark framework
class FakeState:
    def __init__(self, factor):
        self.factor = factor
        self.counters = dict()

    def __bool__(self):
        return True

    def range(self, integer):
        return self.factor

    def set_iteration_time(self, _):
        pass


if __name__ == "__main__":
    benchmark.main()

    # The following code is for debugging a SOFA scene using a GUI
    # import SofaImGui
    # with_gui = True
    # beam_simulation_CUDACholeskySolver(FakeState(5))
