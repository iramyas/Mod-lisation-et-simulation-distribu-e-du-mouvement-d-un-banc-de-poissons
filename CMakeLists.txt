cmake_minimum_required(VERSION 3.20)

project(Modelisation-et-Simulation-distribuee-du-mouvement-d'un-banc-de-poissons
        VERSION 0.1.0
        HOMEPAGE_URL "https://github.com/iramyas/Mod-lisation-et-simulation-distribu-e-du-mouvement-d-un-banc-de-poissons"
        LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COMMON_COMPILE_FLAGS
        $<$<CONFIG:Debug>:-Wall;-Wextra;-Werror;-g;-Wpedantic;-Wshadow;-Wconversion;>
        $<$<CONFIG:RelWithDebInfo>:-O2;-g;-DNDEBUG>
        $<$<CONFIG:Release>:-Wall;-Wextra;-O3;-DNDEBUG;-march=native;-flto>
)

add_executable(main src/main.cpp)