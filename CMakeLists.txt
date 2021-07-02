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
    GIT_TAG master
)
FetchContent_MakeAvailable(googlebenchmark)

sofa_find_package(Sofa.Type REQUIRED)
sofa_find_package(SofaCore REQUIRED)
sofa_find_package(SofaMeshCollision REQUIRED)

set(SOFABENCHMARK_SRC "src")

set(HEADER_FILES "")
set(SOURCE_FILES ${SOFABENCHMARK_SRC}/Main.cpp)

list(APPEND SOURCE_FILES ${SOFABENCHMARK_SRC}/benchmarks/SofaKernel/MatBenchmark.cpp)


list(APPEND HEADER_FILES
    ${SOFABENCHMARK_SRC}/benchmarks/SofaCore/NarrowPhaseDetection.h
)
list(APPEND SOURCE_FILES
    ${SOFABENCHMARK_SRC}/benchmarks/SofaCore/NarrowPhaseDetection.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaHelper/MapPtrStableCompare.cpp
    ${SOFABENCHMARK_SRC}/benchmarks/SofaHelper/AdvancedTimer.cpp
)


add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} benchmark::benchmark)
target_link_libraries(${PROJECT_NAME} Sofa.Type SofaCore SofaMeshCollision)