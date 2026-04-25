cmake_minimum_required(VERSION 3.20)

project(Modelisation-et-Simulation-distribuee-du-mouvement-d'un-banc-de-poissons
        VERSION 1.0.0
        HOMEPAGE_URL "https://github.com/iramyas/Mod-lisation-et-simulation-distribu-e-du-mouvement-d-un-banc-de-poissons"
        LANGUAGES CXX)

cmake_policy(SET CMP0048 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

find_package(GTest REQUIRED)
find_package(MPI REQUIRED)
include(FetchContent)
message(STATUS "MPI found: ${MPI_CXX_LIBRARY}")

if(APPLE)
    set(OpenMP_CXX_FLAGS "-Xclang -fopenmp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")
    set(OpenMP_CXX_INCLUDE_DIR "/opt/homebrew/opt/libomp/include")
endif()

# DÉSACTIVER SFML PAR DÉFAUT
option(USE_SFML "Fetch and build SFML via FetchContent" OFF)

if(USE_SFML)
    set(SFML_BUILD_AUDIO OFF)
    set(SFML_BUILD_NETWORK OFF)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(SFML)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COMMON_COMPILE_FLAGS
        $<$<CONFIG:Debug>:-Wall;-Wextra;-g;-Wpedantic;-Wshadow;-Wconversion;>
        $<$<CONFIG:RelWithDebInfo>:-O2;-g;-DNDEBUG>
        $<$<CONFIG:Release>:-Wall;-Wextra;-O3;-DNDEBUG;-march=native;-flto>
)

add_executable(main_simulation src/main.cpp src/Boid.cpp src/Flock.cpp src/GUI.cpp src/MPIManager.cpp src/DistributedFlock.cpp)
add_executable(test_vector tests/test_vector.cpp tests/kokkos_init.cpp)
add_executable(test_boid tests/test_boid.cpp src/Boid.cpp tests/kokkos_init.cpp)
add_executable(test_flock tests/test_flock.cpp src/Flock.cpp src/Boid.cpp tests/kokkos_init.cpp)
add_executable(test_ui tests/test_ui.cpp src/Flock.cpp src/Boid.cpp tests/kokkos_init.cpp)

set(TEST_MPI_SRC "")
if(EXISTS "${PROJECT_SOURCE_DIR}/tests/test.mpi.cpp")
    set(TEST_MPI_SRC "${PROJECT_SOURCE_DIR}/tests/test.mpi.cpp")
elseif(EXISTS "${PROJECT_SOURCE_DIR}/tests/test_mpi.cpp")
    set(TEST_MPI_SRC "${PROJECT_SOURCE_DIR}/tests/test_mpi.cpp")
endif()

if(TEST_MPI_SRC)
    add_executable(test_mpi ${TEST_MPI_SRC} src/Boid.cpp src/Flock.cpp src/MPIManager.cpp src/DistributedFlock.cpp tests/kokkos_init.cpp)
endif()

message(STATUS "Configuration Kokkos...")
find_package(OpenMP REQUIRED)
message(STATUS "OpenMP found: ${OpenMP_CXX_LIBRARY}")

option(ENABLE_KOKKOS "Activer la parallélisation Kokkos" ON)

if(ENABLE_KOKKOS)
  FetchContent_Declare(
    kokkos
    GIT_REPOSITORY https://github.com/kokkos/kokkos.git
    GIT_TAG master
  )
  FetchContent_GetProperties(kokkos)
  if(NOT kokkos_POPULATED)
    FetchContent_Populate(kokkos)
    add_subdirectory(${kokkos_SOURCE_DIR} ${kokkos_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  target_link_libraries(main_simulation PRIVATE Kokkos::kokkos)
endif()

add_executable(benchmark experiments/benchmark.cpp src/Flock.cpp src/Boid.cpp)
add_executable(benchmark_mpi experiments/benchmark.cpp src/Flock.cpp src/Boid.cpp src/MPIManager.cpp src/DistributedFlock.cpp)
add_executable(benchmark_mpi_comparison experiments/benchmark_mpi_comparison.cpp src/Flock.cpp src/Boid.cpp src/MPIManager.cpp src/DistributedFlock.cpp)

target_include_directories(main_simulation PRIVATE include/)
target_include_directories(test_vector PRIVATE include/)
target_include_directories(test_boid PRIVATE include/) 
target_include_directories(test_flock PRIVATE include/)
target_include_directories(test_ui PRIVATE include/)
target_include_directories(benchmark PRIVATE include/)
target_include_directories(benchmark_mpi PRIVATE include/)
target_include_directories(benchmark_mpi_comparison PRIVATE include/)

if(TARGET test_mpi)
    target_include_directories(test_mpi PRIVATE include/)
endif()

target_compile_options(main_simulation PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(test_vector PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(test_boid PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(test_flock PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(test_ui PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(benchmark PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(benchmark_mpi PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(benchmark_mpi_comparison PRIVATE ${COMMON_COMPILE_FLAGS})

if(TARGET test_mpi)
    target_compile_options(test_mpi PRIVATE ${COMMON_COMPILE_FLAGS})
endif()

# Ajouter flag KOKKOS_ENABLED pour les targets qui utilisent Kokkos
target_compile_definitions(test_vector PRIVATE KOKKOS_ENABLED)
target_compile_definitions(test_boid PRIVATE KOKKOS_ENABLED)
target_compile_definitions(test_flock PRIVATE KOKKOS_ENABLED)
target_compile_definitions(test_ui PRIVATE KOKKOS_ENABLED)

if(TARGET test_mpi)
    target_compile_definitions(test_mpi PRIVATE KOKKOS_ENABLED MPI_ENABLED)
endif()

# Force l'utilisation des headers SFML de FetchContent (SFML 2.6.1) même si SFML est installé sur la machine.
# Sinon, on peut compiler avec des headers SFML 3 (Angle) mais lier avec les libs SFML 2 => erreurs de link.
if(USE_SFML)
    target_include_directories(main_simulation SYSTEM BEFORE PRIVATE ${SFML_SOURCE_DIR}/include)
    target_include_directories(test_flock      SYSTEM BEFORE PRIVATE ${SFML_SOURCE_DIR}/include)
    target_include_directories(test_ui         SYSTEM BEFORE PRIVATE ${SFML_SOURCE_DIR}/include)
    target_include_directories(benchmark       SYSTEM BEFORE PRIVATE ${SFML_SOURCE_DIR}/include)

    target_link_libraries(main_simulation PRIVATE sfml-graphics sfml-window sfml-system Kokkos::kokkos MPI::MPI_CXX)
    target_link_libraries(test_flock PRIVATE GTest::gtest GTest::gtest_main sfml-graphics sfml-window sfml-system Kokkos::kokkos)
    target_link_libraries(test_ui PRIVATE GTest::gtest GTest::gtest_main sfml-graphics sfml-window sfml-system Kokkos::kokkos)
    target_link_libraries(benchmark PRIVATE sfml-graphics sfml-window sfml-system Kokkos::kokkos)
    target_link_libraries(benchmark_mpi PRIVATE sfml-graphics sfml-window sfml-system Kokkos::kokkos MPI::MPI_CXX)
    target_link_libraries(benchmark_mpi_comparison PRIVATE Kokkos::kokkos MPI::MPI_CXX)
    target_link_libraries(test_vector PRIVATE GTest::gtest GTest::gtest_main Kokkos::kokkos)
    target_link_libraries(test_boid PRIVATE GTest::gtest GTest::gtest_main Kokkos::kokkos)
else()
    # fallback link lines when SFML disabled
    target_link_libraries(main_simulation PRIVATE Kokkos::kokkos MPI::MPI_CXX)
    target_link_libraries(test_vector PRIVATE GTest::gtest GTest::gtest_main Kokkos::kokkos)
    target_link_libraries(test_boid PRIVATE GTest::gtest GTest::gtest_main Kokkos::kokkos)
    target_link_libraries(test_flock PRIVATE GTest::gtest GTest::gtest_main Kokkos::kokkos)
    target_link_libraries(test_ui PRIVATE GTest::gtest GTest::gtest_main Kokkos::kokkos)
    target_link_libraries(benchmark PRIVATE Kokkos::kokkos)
    target_link_libraries(benchmark_mpi PRIVATE Kokkos::kokkos MPI::MPI_CXX)
    target_link_libraries(benchmark_mpi_comparison PRIVATE Kokkos::kokkos MPI::MPI_CXX)
    
    if(TARGET test_mpi)
        target_link_libraries(test_mpi PRIVATE GTest::gtest GTest::gtest_main Kokkos::kokkos MPI::MPI_CXX)
    endif()
endif()

set_target_properties(main_simulation PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "main_simulation.bin"
)
set_target_properties(test_vector PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "test_vector.bin"  
)
set_target_properties(test_boid PROPERTIES  
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "test_boid.bin"
)
set_target_properties(test_flock PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin 
        OUTPUT_NAME "test_flock.bin"
)
set_target_properties(test_ui PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin 
        OUTPUT_NAME "test_ui.bin"
)
set_target_properties(benchmark PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "benchmark.bin"
)
set_target_properties(benchmark_mpi PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "benchmark_mpi.bin"
)

set_target_properties(benchmark_mpi_comparison PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "benchmark_mpi_comparison.bin"
)

if(TARGET test_mpi)
    set_target_properties(test_mpi PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "test_mpi.bin"
    )
endif()

enable_testing()
gtest_discover_tests(test_vector)
gtest_discover_tests(test_boid)
gtest_discover_tests(test_flock)
gtest_discover_tests(test_ui)

if(TARGET test_mpi)
    gtest_discover_tests(test_mpi)
endif()

# GUARD ALL test_mpi operations inside if(TARGET test_mpi)
if(TARGET test_mpi)
    target_include_directories(test_mpi PRIVATE include/)
    target_compile_options(test_mpi PRIVATE ${COMMON_COMPILE_FLAGS})
    target_compile_definitions(test_mpi PRIVATE KOKKOS_ENABLED MPI_ENABLED)
    target_link_libraries(test_mpi PRIVATE GTest::gtest GTest::gtest_main Kokkos::kokkos MPI::MPI_CXX)
    set_target_properties(test_mpi PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "test_mpi.bin"
    )
    gtest_discover_tests(test_mpi)
endif()


# Test de cohérence (séquentiel vs distribué)
set(TEST_COHERENCE_SRC "${PROJECT_SOURCE_DIR}/tests/test_coherence.cpp")
if(EXISTS "${TEST_COHERENCE_SRC}")
    add_executable(test_coherence ${TEST_COHERENCE_SRC} src/Boid.cpp src/Flock.cpp src/MPIManager.cpp src/DistributedFlock.cpp tests/kokkos_init.cpp)
    target_include_directories(test_coherence PRIVATE include/)
    target_compile_options(test_coherence PRIVATE ${COMMON_COMPILE_FLAGS})
    if(ENABLE_KOKKOS)
        target_link_libraries(test_coherence PRIVATE Kokkos::kokkos)
    endif()
    target_link_libraries(test_coherence PRIVATE MPI::MPI_CXX OpenMP::OpenMP_CXX)
    set_target_properties(test_coherence PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "test_coherence.bin"
    )
endif()
