#include <gtest/gtest.h>
#include "Flock.h"

using namespace simulation;

TEST(FlockNeighbors, NeighborRadiusWorks) {
    Flock flock(200.f, 200.f, 20.f);

    Boid a(10.f, 10.f);
    Boid b(20.f, 10.f);

    flock.addBoid(a);
    flock.addBoid(b);

    flock.neighborRadius = 15.f;
    flock.grid.clear();
    for (auto &bo : flock.boids) flock.grid.addBoid(&bo);

    auto neighbors = flock.getNeighbors(&flock.boids[0]);
    EXPECT_EQ(neighbors.size(), 1);

    flock.neighborRadius = 5.f;
    flock.grid.clear();
    for (auto &bo : flock.boids) flock.grid.addBoid(&bo);

    auto neighbors2 = flock.getNeighbors(&flock.boids[0]);
    EXPECT_EQ(neighbors2.size(), 0);
}
