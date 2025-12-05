cmake_minimum_required(VERSION 3.20)

project(Modelisation-et-Simulation-distribuee-du-mouvement-d'un-banc-de-poissons
        VERSION 0.2.0
        HOMEPAGE_URL "https://github.com/iramyas/Mod-lisation-et-simulation-distribu-e-du-mouvement-d-un-banc-de-poissons"
        LANGUAGES CXX)

find_package(GTest REQUIRED)

# Récupération de SFML pour l'interface graphique
set(SFML_BUILD_AUDIO OFF)
set(SFML_BUILD_NETWORK OFF)
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COMMON_COMPILE_FLAGS
        $<$<CONFIG:Debug>:-Wall;-Wextra;-g;-Wpedantic;-Wshadow;-Wconversion;>
        $<$<CONFIG:RelWithDebInfo>:-O2;-g;-DNDEBUG>
        $<$<CONFIG:Release>:-Wall;-Wextra;-O3;-DNDEBUG;-march=native;-flto>
)

add_executable(main_simulation src/main.cpp src/Boid.cpp)
add_executable(test_runner tests/test_vector.cpp)
add_executable(test_boid tests/test_boid.cpp src/Boid.cpp)
add_executable(test_flock tests/test_flock.cpp src/Flock.cpp src/Boid.cpp)

target_include_directories(main_simulation PRIVATE include/)
target_include_directories(test_runner PRIVATE include/)
target_include_directories(test_boid PRIVATE include/) 
target_include_directories(test_flock PRIVATE include/)

target_compile_options(main_simulation PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(test_runner PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(test_boid PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(test_flock PRIVATE ${COMMON_COMPILE_FLAGS})

target_link_libraries(main_simulation PRIVATE sfml-graphics sfml-window sfml-system)
target_link_libraries(test_runner PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test_boid PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(test_flock PRIVATE GTest::gtest GTest::gtest_main)

set_target_properties(main_simulation PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "main_simulation.bin"
)
set_target_properties(test_runner PROPERTIES
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

enable_testing()
gtest_discover_tests(test_runner)
gtest_discover_tests(test_boid)
gtest_discover_tests(test_flock)