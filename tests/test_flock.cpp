#include "gtest/gtest.h"
#include <vector>
#include "Flock.h"
#include "boid.h"

using simulation::Flock;
using simulation::Boid;

/*
    TEST DU CONSTRUCTEUR
*/

TEST(FlockTest, ConstructeurInitialiseGrid){

    Flock flock(200.0f, 200.0f, 20.0f);

    // Vérifie que les dimensions sont correctes
    EXPECT_EQ(flock.grid.gridWidth, 10);
    EXPECT_EQ(flock.grid.gridHeight, 10);
    EXPECT_FLOAT_EQ(flock.grid.cellSize, 20.0f);

    // Grille devrait être vide
    EXPECT_TRUE(flock.grid.cells.size() == 100);

}

/*
    TEST D'AJOUT DE BOIDS
*/

TEST(FlockTest, AddBoidIncrementsList){

    Flock flock(200.0f, 200.0f, 20.0f);

    EXPECT_EQ(flock.boids.size(), 0);

    Boid b1(10.0f, 10.0f);
    flock.addBoid(b1);

    EXPECT_EQ(flock.boids.size(), 1);

    Boid b2(30.0f, 50.0f);
    flock.addBoid(b2);

    EXPECT_EQ(flock.boids.size(), 2);

}

/*
    TEST DU GRIDSPATIAL: AJOUT + QUERY
*/

TEST(FlockTest, SpatialGridAddBoidPlacesCorrectCell){

    Flock flock(100.0f, 100.0f, 10.0f);

    Boid b1(15.0f, 25.0f);
    flock.addBoid(b1);

    flock.grid.clear();
    flock.grid.addBoid(&flock.boids[0]);

    // Cellule pour (15,25) = (1,2)
    int cellIndex = 2 * flock.grid.gridWidth + 1;

    EXPECT_EQ(flock.grid.cells[cellIndex].size(), 1);
    EXPECT_EQ(flock.grid.cells[cellIndex][0], &flock.boids[0]);

}

TEST(FlockTest, QueryNeighborsReturnsCorrectBoids){

    Flock flock(100.0f, 100.0f, 20.0f);

    Boid b1(10.0f, 10.0f);
    Boid b2(12.0f, 15.0f);  // voisin proche
    Boid b3(80.0f, 80.0f);  // très loin → ignoré

    flock.addBoid(b1);
    flock.addBoid(b2);
    flock.addBoid(b3);

    flock.grid.clear();
    for (Boid &b : flock.boids) flock.grid.addBoid(&b);

    auto neighbors = flock.grid.queryNeighbors(&flock.boids[0]);

    EXPECT_TRUE(std::find(neighbors.begin(), neighbors.end(), &flock.boids[1]) != neighbors.end());
    EXPECT_TRUE(std::find(neighbors.begin(), neighbors.end(), &flock.boids[2]) == neighbors.end());

}

/*
    TEST DE GETNEIGHBORS
*/

TEST(FlockTest, GetNeighborsUsesGrid){

    Flock flock(200.0f, 200.0f, 20.0f);

    Boid b1(10.0f, 10.0f);
    Boid b2(15.0f, 12.0f);

    flock.addBoid(b1);
    flock.addBoid(b2);

    flock.grid.clear();
    for (Boid &b : flock.boids) flock.grid.addBoid(&b);

    auto list = flock.getNeighbors(&flock.boids[0]);

    EXPECT_EQ(list.size(), 1);
    EXPECT_EQ(list[0], &flock.boids[1]);

}

/*
    TEST DE UPDATEALL
*/

TEST(FlockTest, UpdateAllUpdatesPositions){

    Flock flock(200.0f, 200.0f, 20.0f);

    Boid b1(0.0f, 0.0f);
    b1.velocity = {1.0f, 0.0f};
    flock.addBoid(b1);

    flock.updateAll(1.0f);

    EXPECT_FLOAT_EQ(flock.boids[0].position.x, 1.0f);
    EXPECT_FLOAT_EQ(flock.boids[0].position.y, 0.0f);

}

TEST(FlockTest, UpdateAllRebuildsGrid){

    Flock flock(100.0f, 100.0f, 10.0f);

    Boid b1(5.0f, 5.0f);
    flock.addBoid(b1);

    flock.updateAll(1.0f);

    int cellIndex = 0; // (0,0)
    EXPECT_EQ(flock.grid.cells[cellIndex].size(), 1);
    
}
