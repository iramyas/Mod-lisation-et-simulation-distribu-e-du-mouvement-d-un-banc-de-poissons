#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "Flock.h"
#include "boid.h"

using simulation::Boid;
using simulation::Flock;

namespace {

struct Args {
    std::string mode = "grid"; // grid | naive
    int N = 500;
    float radius = 50.f;
    int steps = 300;
    float dt = 0.016f;
    float width = 1100.f;
    float height = 700.f;
    float cellSize = 40.f;
    bool header = false;
};

Args parseArgs(int argc, char** argv) {
    Args a;
    for (int i = 1; i < argc; ++i) {
        std::string k = argv[i];
        auto needValue = [&](const char* name) {
            if (i + 1 >= argc) {
                std::cerr << "Missing value for " << name << "\n";
                std::exit(2);
            }
            return std::string(argv[++i]);
        };

        if (k == "--mode") a.mode = needValue("--mode");
        else if (k == "--N") a.N = std::stoi(needValue("--N"));
        else if (k == "--radius") a.radius = std::stof(needValue("--radius"));
        else if (k == "--steps") a.steps = std::stoi(needValue("--steps"));
        else if (k == "--dt") a.dt = std::stof(needValue("--dt"));
        else if (k == "--width") a.width = std::stof(needValue("--width"));
        else if (k == "--height") a.height = std::stof(needValue("--height"));
        else if (k == "--cellSize") a.cellSize = std::stof(needValue("--cellSize"));
        else if (k == "--header") a.header = true;
        else if (k == "--help" || k == "-h") {
            std::cout
                << "Usage: benchmark.bin [--header] --mode grid|naive --N <int> --radius <float> --steps <int> [--cellSize <float>] [--dt <float>]\n";
            std::exit(0);
        }
    }

    if (a.N <= 0 || a.steps <= 0 || a.radius < 0.f || a.cellSize <= 0.f) {
        std::cerr << "Invalid arguments. Use --help.\n";
        std::exit(2);
    }

    return a;
}

void runGrid(const Args& a) {
    Flock flock(a.width, a.height, a.cellSize);
    flock.setNeighborRadius(a.radius);
    flock.populateRandom(a.N);

    // warmup
    for (int i = 0; i < 10; ++i) flock.updateAll(a.dt);

    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < a.steps; ++i) flock.updateAll(a.dt);
    const auto t1 = std::chrono::steady_clock::now();

    const double msTotal = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double msPerStep = msTotal / static_cast<double>(a.steps);

    std::cout << "grid," << a.N << "," << a.radius << "," << a.cellSize << "," << a.steps << "," << msTotal << "," << msPerStep << "\n";
}

void runNaive(const Args& a) {
    Flock flock(a.width, a.height, a.cellSize);
    flock.populateRandom(a.N);

    // On force un rayon unique pour être comparable
    for (auto& b : flock.boids) b.perceptionRadius = a.radius;

    std::vector<Boid*> all;
    all.reserve(flock.boids.size());

    auto step = [&] {
        all.clear();
        for (auto& b : flock.boids) all.push_back(&b);

        for (auto& b : flock.boids) {
            b.perceptionRadius = a.radius;
            auto neighbors = b.getNeighbors(all); // O(N^2)

            b.applyForce(b.separate(neighbors) * flock.sepWeight);
            b.applyForce(b.align(neighbors)    * flock.aliWeight);
            b.applyForce(b.cohesion(neighbors) * flock.cohWeight);
            b.update(a.dt);

            // wrap-around (comme dans main.cpp)
            if (b.position.x < 0)         b.position.x += a.width;
            if (b.position.x > a.width)   b.position.x -= a.width;
            if (b.position.y < 0)         b.position.y += a.height;
            if (b.position.y > a.height)  b.position.y -= a.height;
        }
    };

    // warmup
    for (int i = 0; i < 10; ++i) step();

    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < a.steps; ++i) step();
    const auto t1 = std::chrono::steady_clock::now();

    const double msTotal = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double msPerStep = msTotal / static_cast<double>(a.steps);

    std::cout << "naive," << a.N << "," << a.radius << "," << a.cellSize << "," << a.steps << "," << msTotal << "," << msPerStep << "\n";
}

} // namespace

int main(int argc, char** argv) {
    std::srand(0); // reproductible

    const Args a = parseArgs(argc, argv);

    if (a.header) {
        std::cout << "mode,N,radius,cellSize,steps,ms_total,ms_per_step\n";
    }

    if (a.mode == "grid") runGrid(a);
    else if (a.mode == "naive") runNaive(a);
    else {
        std::cerr << "Unknown --mode: " << a.mode << " (expected grid|naive)\n";
        return 2;
    }

    return 0;
}
