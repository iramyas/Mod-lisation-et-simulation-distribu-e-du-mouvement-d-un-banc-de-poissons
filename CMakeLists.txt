cmake_minimum_required(VERSION 3.20)

project(Modelisation-et-Simulation-distribuee-du-mouvement-d'un-banc-de-poissons
        VERSION 0.1.0
        HOMEPAGE_URL "https://github.com/iramyas/Mod-lisation-et-simulation-distribu-e-du-mouvement-d-un-banc-de-poissons"
        LANGUAGES CXX)

find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COMMON_COMPILE_FLAGS
        $<$<CONFIG:Debug>:-Wall;-Wextra;-Werror;-g;-Wpedantic;-Wshadow;-Wconversion;>
        $<$<CONFIG:RelWithDebInfo>:-O2;-g;-DNDEBUG>
        $<$<CONFIG:Release>:-Wall;-Wextra;-O3;-DNDEBUG;-march=native;-flto>
)

add_executable(main_simulation src/main.cpp)
add_executable(test_runner tests/test_vector.cpp)

target_include_directories(main_simulation PRIVATE include/)
target_include_directories(test_runner PRIVATE include/)

target_compile_options(main_simulation PRIVATE ${COMMON_COMPILE_FLAGS})
target_compile_options(test_runner PRIVATE ${COMMON_COMPILE_FLAGS})

target_link_libraries(test_runner PRIVATE GTest::gtest GTest::gtest_main)

set_target_properties(main_simulation PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "main_simulation.bin"
)
set_target_properties(test_runner PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        OUTPUT_NAME "test.bin"
)

enable_testing()
gtest_discover_tests(test_runner)