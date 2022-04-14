cmake_minimum_required(VERSION 3.14)
project(SofaBenchmarkScenes)

sofa_find_package(SofaComponentAll REQUIRED)

set(SOFABENCHMARKSCENES_SRC "src")

set(HEADER_FILES
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/BenchScene.h
)
set(SOURCE_FILES
    ${SOFABENCHMARKSCENES_SRC}/Main.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/SimpleScene.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/mass/DiagonalMass.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/mass/MeshMatrixMass.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/fem/StandardTetrahedralFEMForceField.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/fem/StiffSpringForceField.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/fem/TetrahedronFEMForceField.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/fem/TriangularFEMForceFieldOptim.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/linearsolver/SparseLDLSolver.cpp
    ${SOFABENCHMARKSCENES_SRC}/SofaBenchmarkScenes/linearsolver/SparseLUSolver.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC benchmark::benchmark)
target_link_libraries(${PROJECT_NAME} PUBLIC SofaComponentAll)
target_include_directories(${PROJECT_NAME} PUBLIC ${SOFABENCHMARKSCENES_SRC})

# regroup benchmark stuff into its own IDE folder
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER SofaBenchmark)
