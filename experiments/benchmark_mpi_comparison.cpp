#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <mpi.h>

#include "Flock.h"
#include "DistributedFlock.h"
#include "boid.h"

using simulation::Boid;
using simulation::Flock;
using simulation::DistributedFlock;

namespace {

struct Args {
    int N = 500;
    float radius = 50.f;
    int steps = 100;
    float dt = 0.016f;
    float width = 1100.f;
    float height = 700.f;
    float cellSize = 40.f;
    bool csv = false;
    bool verbose = true;
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

        if (k == "--N") a.N = std::stoi(needValue("--N"));
        else if (k == "--radius") a.radius = std::stof(needValue("--radius"));
        else if (k == "--steps") a.steps = std::stoi(needValue("--steps"));
        else if (k == "--dt") a.dt = std::stof(needValue("--dt"));
        else if (k == "--width") a.width = std::stof(needValue("--width"));
        else if (k == "--height") a.height = std::stof(needValue("--height"));
        else if (k == "--cellSize") a.cellSize = std::stof(needValue("--cellSize"));
        else if (k == "--csv") a.csv = true;
        else if (k == "--quiet") a.verbose = false;
        else if (k == "--help" || k == "-h") {
            std::cout
                << "Usage: benchmark_mpi_comparison.bin [--csv] [--quiet] --N <int> --radius <float> --steps <int>\n"
                << "       [--dt <float>] [--width <float>] [--height <float>] [--cellSize <float>]\n"
                << "\nOptions:\n"
                << "  --N <int>          Number of boids (default: 500)\n"
                << "  --radius <float>   Perception radius (default: 50)\n"
                << "  --steps <int>      Simulation steps (default: 100)\n"
                << "  --dt <float>       Time step (default: 0.016)\n"
                << "  --width <float>    Simulation width (default: 1100)\n"
                << "  --height <float>   Simulation height (default: 700)\n"
                << "  --cellSize <float> Grid cell size (default: 40)\n"
                << "  --csv              Output as CSV\n"
                << "  --quiet            Minimal output (for batch processing)\n";
            std::exit(0);
        }
    }

    if (a.N <= 0 || a.steps <= 0 || a.radius < 0.f || a.cellSize <= 0.f) {
        std::cerr << "Invalid arguments. Use --help.\n";
        std::exit(2);
    }

    return a;
}

struct BenchmarkResult {
    std::string implementation;
    int N;
    float radius;
    int steps;
    double totalMs;
    double msPerStep;
    double msPerBoidStep;  // ms per step per boid
};

/**
 * @brief Run sequential (non-MPI) benchmark
 */
BenchmarkResult benchmarkSequential(const Args& a) {
    Flock flock(a.width, a.height, a.cellSize);
    flock.setNeighborRadius(a.radius);
    flock.populateRandom(a.N);

    // Warmup
    for (int i = 0; i < 5; ++i) {
        flock.updateAll(a.dt);
    }

    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < a.steps; ++i) {
        flock.updateAll(a.dt);
    }
    const auto t1 = std::chrono::steady_clock::now();

    const double msTotal = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double msPerStep = msTotal / static_cast<double>(a.steps);
    const double msPerBoidStep = msPerStep / static_cast<double>(a.N);

    return {
        "Sequential",
        a.N,
        a.radius,
        a.steps,
        msTotal,
        msPerStep,
        msPerBoidStep
    };
}

/**
 * @brief Run MPI-distributed benchmark
 */
BenchmarkResult benchmarkDistributed(const Args& a) {
    DistributedFlock flock(a.width, a.height, a.cellSize);
    flock.setNeighborRadius(a.radius);
    flock.populateRandomDistributed(a.N);

    // Warmup
    for (int i = 0; i < 5; ++i) {
        flock.updateAllDistributed(a.dt);
    }

    const auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < a.steps; ++i) {
        flock.updateAllDistributed(a.dt);
    }
    const auto t1 = std::chrono::steady_clock::now();

    const double msTotal = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double msPerStep = msTotal / static_cast<double>(a.steps);
    const double msPerBoidStep = msPerStep / static_cast<double>(a.N);

    return {
        "Distributed (MPI)",
        a.N,
        a.radius,
        a.steps,
        msTotal,
        msPerStep,
        msPerBoidStep
    };
}

void printResultCSV(const BenchmarkResult& r) {
    std::cout << r.implementation << ","
              << r.N << ","
              << r.radius << ","
              << r.steps << ","
              << std::fixed << std::setprecision(6)
              << r.totalMs << ","
              << r.msPerStep << ","
              << r.msPerBoidStep << "\n";
}

void printResultHuman(const BenchmarkResult& r) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "Implementation:  " << r.implementation << "\n";
    std::cout << "Boids (N):       " << r.N << "\n";
    std::cout << "Perception rad:  " << r.radius << "\n";
    std::cout << "Steps:           " << r.steps << "\n";
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Total time:      " << r.totalMs << " ms\n";
    std::cout << "Per step:        " << r.msPerStep << " ms\n";
    std::cout << "Per boid/step:   " << r.msPerBoidStep << " µs\n";
    std::cout << std::string(60, '=') << "\n";
}

}  // namespace

int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Args args = parseArgs(argc, argv);

    bool isSequential = (size == 1);

    if (rank == 0 && args.verbose) {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "Benchmark: Sequential vs MPI-Distributed\n";
        std::cout << "MPI Ranks: " << size << "\n";
        std::cout << std::string(60, '=') << "\n";
    }

    std::vector<BenchmarkResult> results;

    // Always run sequential on rank 0
    if (rank == 0) {
        if (args.verbose) std::cout << "\n[1/2] Running Sequential benchmark...\n";
        results.push_back(benchmarkSequential(args));
    }

    // Synchronize before running distributed version
    MPI_Barrier(MPI_COMM_WORLD);

    // Run distributed version on all ranks
    if (args.verbose && rank == 0) std::cout << "[2/2] Running Distributed (MPI) benchmark...\n";
    BenchmarkResult distResult = benchmarkDistributed(args);

    // Gather results on rank 0
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        results.push_back(distResult);

        if (args.csv) {
            // CSV header
            std::cout << "implementation,N,radius,steps,total_ms,ms_per_step,us_per_boid_step\n";
            for (const auto& r : results) {
                printResultCSV(r);
            }
        } else {
            // Human-readable output
            for (const auto& r : results) {
                printResultHuman(r);
            }

            // Summary comparison
            if (results.size() == 2) {
                double speedup = results[0].msPerStep / results[1].msPerStep;
                std::cout << "\nComparison:\n";
                std::cout << "  Speedup (Sequential / MPI): " 
                          << std::fixed << std::setprecision(2) << speedup << "x\n";
                if (speedup > 1.0) {
                    std::cout << "  → MPI is FASTER\n";
                } else {
                    std::cout << "  → Sequential is FASTER (might need more processes)\n";
                }
            }
        }
    }

    MPI_Finalize();
    return 0;
}
