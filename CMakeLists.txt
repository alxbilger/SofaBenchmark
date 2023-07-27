cmake_minimum_required(VERSION 3.14)
project(SofaBenchmark)

include(FetchContent)

# By default, google benchmark requires gtest as a dependency
# The gtest sources in Sofa.Testing are not compatible with google benchmark
# because of the way gtest is found in the cmake script
# Tests are disabled, but they can be enabled by providing either:
# - BENCHMARK_DOWNLOAD_DEPENDENCIES set to true
# - GOOGLETEST_PATH to an existing directory where gtest has been checked out
set(BENCHMARK_ENABLE_GTEST_TESTS FALSE)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG main
)
FetchContent_MakeAvailable(googlebenchmark)

find_package(Sofa.Config REQUIRED)
sofa_find_package(Sofa.Type REQUIRED)
sofa_find_package(Sofa.Core REQUIRED)
sofa_find_package(Sofa.Simulation.Graph REQUIRED)
sofa_find_package(Sofa.Component.Collision.Geometry REQUIRED)

set(SOFABENCHMARK_SRC "src")

set(HEADER_FILES "")
set(SOURCE_FILES ${SOFABENCHMARK_SRC}/Main.cpp)

list(APPEND HEADER_FILES
    ${SOFABENCHMARK_SRC}/benchmarks/SofaCore/NarrowPhaseDetection.h
    ${SOFABENCHMARK_SRC}/utils/RandomValuePool.h
    ${SOFABENCHMARK_SRC}/utils/SparseMatrix.h
    ${SOFABENCHMARK_SRC}/utils/thread_pool.hpp
)
list(APPEND SOURCE_FILES
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.LinearAlgebra/SparseMatrixCompression.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.LinearAlgebra/SparseMatrixMulTranspose.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.LinearAlgebra/SparseMatrixProduct.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.Type/Matrix.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.Type/Vec.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.Type/Quat.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.Type/fixed_array.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaBaseLinearSolver/CompressedRowSparse.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaCore/Data.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaCore/MultiVecId.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaCore/NarrowPhaseDetection.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaCore/ReadAccessor.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaDefaultType/MapMapSparseMatrix.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaHelper/AdvancedTimer.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaHelper/MapPtrStableCompare.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaSimulationCore/TaskScheduler.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.Component.SolidMechanics.FEM.Elastic/HexahedronFEMForceField_benchmark.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/Sofa.Component.SolidMechanics.FEM.Elastic/TetrahedronFEMForceField_benchmark.cpp
)

option(SOFABENCHMARK_BUILD_BENCH_SCENES "Add benchmarking SOFA scenes." ON)
if(SOFABENCHMARK_BUILD_BENCH_SCENES)
    add_subdirectory(SofaBenchmarkScenes)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC benchmark::benchmark)
target_link_libraries(${PROJECT_NAME} PUBLIC Sofa.Type Sofa.Core Sofa.Simulation.Graph Sofa.Component.Collision.Geometry)
target_include_directories(${PROJECT_NAME} PUBLIC ${SOFABENCHMARK_SRC})

# regroup benchmark stuff into its own IDE folder
set_target_properties(benchmark PROPERTIES FOLDER SofaBenchmark)
set_target_properties(benchmark_main PROPERTIES FOLDER SofaBenchmark)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER SofaBenchmark)
