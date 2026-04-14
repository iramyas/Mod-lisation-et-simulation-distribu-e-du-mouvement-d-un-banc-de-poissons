#include <gtest/gtest.h>
#include <mpi.h>
#include <iomanip>
#include <iostream>

#include "DistributedFlock.h"

using namespace simulation;

class MPITest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // MPI déjà initialisé par le main de GTest
    }
    static void TearDownTestSuite() {
        
    }
};

TEST_F(MPITest, DomainDecomposition) {
    DistributedFlock flock(1200.0f, 800.0f, 50.0f);
    const auto& domain = flock.getMPIManager().getLocalDomain();
    
    EXPECT_GE(domain.maxX, domain.minX);
    EXPECT_GE(domain.maxY, domain.minY);
}

TEST_F(MPITest, PopulateRandomDistributed) {
    DistributedFlock flock(1200.0f, 800.0f, 50.0f);
    flock.populateRandomDistributed(100);
    
    EXPECT_GT(flock.boids.size(), 0);
}

TEST_F(MPITest, HaloExchange) {
    DistributedFlock flock(1200.0f, 800.0f, 50.0f);
    flock.populateRandomDistributed(100);
    flock.performHaloExchange();
    
    EXPECT_GE(flock.getHaloBoids().size(), 0);
}

TEST_F(MPITest, SyncGlobalCount) {
    MPI_Barrier(MPI_COMM_WORLD);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    DistributedFlock flock(1200.0f, 800.0f, 50.0f);
    flock.populateRandomDistributed(100);
    flock.syncGlobalBoidCount();
    
    int globalCount = flock.getGlobalBoidCount();
    EXPECT_GT(globalCount, 0);
    EXPECT_GE(globalCount, (int)flock.boids.size());

    MPI_Barrier(MPI_COMM_WORLD);
}

// Main qui gère MPI_Init/Finalize
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    
    MPI_Finalize();
    return result;
}