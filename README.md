# SofaBenchmark
A [Sofa](https://github.com/sofa-framework/sofa) application to benchmark [Sofa](https://github.com/sofa-framework/sofa) code using [google benchmark](https://github.com/google/benchmark)

## Introduction

This application must be compiled along with Sofa (https://github.com/sofa-framework/sofa). It is used to benchmark code from [Sofa](https://github.com/sofa-framework/sofa).

## Compilation

### Prerequisites

- Being able to build Sofa: https://www.sofa-framework.org/community/doc/
- git
- CMake

### Instructions

To add the SofaBenchmark target to the Sofa solution:
- Clone SofaBenchmark somewhere on your system
- At the same level, if it's not done yet, create `CMakeLists.txt`
- In `CMakeLists.txt`, add the following line: `sofa_add_application(SofaBenchmark SofaBenchmark)`
- Enter the path where you cloned SofaBenchmark in the Sofa CMake variable `SOFA_EXTERNAL_DIRECTORIES` (see https://www.sofa-framework.org/community/doc/plugins/build-a-plugin-from-sources/)
- Run CMake
- SofaBenchmark should appear as a new target

## Code Example

The application uses the micro-benchmarking library google benchmark (https://github.com/google/benchmark). See the repository [readme](https://github.com/google/benchmark#readme) for generic examples.

The following example benchmarks the call to `NarrowPhaseDetection::getDetectionOutputs`
```c++
void BM_NarrowPhaseDetection_getDetectionOutputs(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        auto narrowPhaseDetection = New<sofa::component::collision::EmptyNarrowPhaseDetection>();

        sofa::helper::vector<sofa::component::collision::PointCollisionModel<sofa::defaulttype::Vec3Types>::SPtr> collisionModels;
        for (unsigned int i = 0; i < state.range(0); ++i)
        {
            collisionModels.push_back(New<sofa::component::collision::PointCollisionModel<sofa::defaulttype::Vec3Types> >());
        }

        state.ResumeTiming();
        for (auto a : collisionModels)
        {
            for (auto b : collisionModels)
            {
                narrowPhaseDetection->getDetectionOutputs(a.get(), b.get());
            }
        }
    }
}
```

## Output

The benchmark of `NarrowPhaseDetection::getDetectionOutputs` defined in the previous section gives the following output:

```
------------------------------------------------------------------------------------------
Benchmark                                                Time             CPU   Iterations
------------------------------------------------------------------------------------------
BM_NarrowPhaseDetection_getDetectionOutputs/8         38.1 us         38.1 us        17905
BM_NarrowPhaseDetection_getDetectionOutputs/16         241 us          241 us         2860
BM_NarrowPhaseDetection_getDetectionOutputs/32        1423 us         1423 us          498
BM_NarrowPhaseDetection_getDetectionOutputs/64        8453 us         8454 us           87
BM_NarrowPhaseDetection_getDetectionOutputs/128      44993 us        44994 us           15
BM_NarrowPhaseDetection_getDetectionOutputs/256     238668 us       238664 us            3
```

## Benchmark SOFA Scenes

A second application is available: SofaBenchmarkScenes.
It must be enabled in CMake with the variable `SOFABENCHMARK_BUILD_BENCH_SCENES`.
While SofaBenchmark focus on code snippet, this application is specific to benchmark entire SOFA scenes.

### Output

An example of output for SofaBenchmarkScenes is:

```
-------------------------------------------------------------------------------------------------------------------
Benchmark                                                         Time             CPU   Iterations UserCounters...
-------------------------------------------------------------------------------------------------------------------
BM_Scene_bench_SimulationFactor<SparseLDLSolverScene>/2        1583 ms         1562 ms            1 FPS=128/s frame=7.8125ms
BM_Scene_bench_SimulationFactor<SparseLDLSolverScene>/4        3192 ms         3219 ms            1 FPS=124.272/s frame=8.04688ms
BM_Scene_bench_SimulationFactor<SparseLDLSolverScene>/8        6325 ms         6281 ms            1 FPS=127.363/s frame=7.85156ms
BM_Scene_bench_SimulationFactor<SparseLDLSolverScene>/16      12743 ms        12766 ms            1 FPS=125.337/s frame=7.97852ms
BM_Scene_bench_StepFactor<SparseLDLSolverScene>/512            3940 ms         3906 ms            1 FPS=131.072/s frame=7.62939ms
BM_Scene_bench_StepFactor<SparseLDLSolverScene>/1024           7810 ms         7781 ms            1 FPS=131.598/s frame=7.59888ms
BM_Scene_bench_StepFactor<SparseLDLSolverScene>/2048          15717 ms        15719 ms            1 FPS=130.29/s frame=7.67517ms

```

