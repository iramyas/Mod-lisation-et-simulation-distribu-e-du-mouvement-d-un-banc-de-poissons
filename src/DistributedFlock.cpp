#include "DistributedFlock.h"
#include <random>

namespace simulation {

DistributedFlock::DistributedFlock(float width, float height, float cellSize)
    : Flock(width, height, cellSize), globalBoidCount(0) {
}

void DistributedFlock::performHaloExchange() {
    mpiManager.haloExchange(boids, neighborRadius, haloBoids);
}

void DistributedFlock::performMigration() {
    std::vector<Boid> remaining, incoming;
    mpiManager.migrateBoidsAllToAll(boids, remaining, incoming);
    boids = remaining;
    boids.insert(boids.end(), incoming.begin(), incoming.end());
}

void DistributedFlock::syncGlobalBoidCount() {
    globalBoidCount = mpiManager.syncGlobalCount(boids.size());
}

void DistributedFlock::gatherAllBoids(std::vector<Boid>& globalBoids) {
    mpiManager.gatherBoids(boids, globalBoids);
}

void DistributedFlock::scatterAllBoids(const std::vector<Boid>& globalBoids) {
    mpiManager.scatterBoids(globalBoids, boids);
}

void DistributedFlock::populateRandomDistributed(int totalN) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int localN = totalN / mpiManager.getSize();
    const auto& domain = mpiManager.getLocalDomain();
    
    std::random_device rd;
    std::mt19937 gen(rd() + rank);
    std::uniform_real_distribution<> disX(domain.minX, domain.maxX);
    std::uniform_real_distribution<> disY(domain.minY, domain.maxY);
    
    for (int i = 0; i < localN; ++i) {
        float x = disX(gen);
        float y = disY(gen);
        boids.emplace_back(x, y);
    }
}

void DistributedFlock::updateAllDistributed(float deltaTime) {
    performHaloExchange();
    rebuildSpatialGrid();
    // Call parent class update logic or implement local logic
    performMigration();
    syncGlobalBoidCount();
}

void DistributedFlock::rebuildSpatialGrid() {
    // Spatial grid rebuild logic
    // spatialGrid operations would go here if spatialGrid member is defined
}
} // namespace simulation