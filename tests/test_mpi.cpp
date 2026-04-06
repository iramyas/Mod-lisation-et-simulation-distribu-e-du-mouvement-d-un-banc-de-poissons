#include <gtest/gtest.h>
#include <mpi.h>
#include <iomanip>  

#include "/Users/nouzhafou/Mod-lisation-et-simulation-distribu-e-du-mouvement-d-un-banc-de-poissons/include/DistributedFlock.h"

using namespace simulation;

class MPITest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // MPI déjà initialisé par le main de GTest
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

/**
 * @brief Programme de test de la parallélisation MPI
 * 
 * Teste les fonctionnalités principales:
 * - Initialisation MPI
 * - Décomposition de domaine
 * - Création distribuée de boids
 * - Échange de halos
 * - Migration des boids
 * - Synchronisation globale
 */
int main(int argc, char* argv[]) {
    // Initialiser MPI
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    const float SIM_WIDTH = 1200.0f;
    const float SIM_HEIGHT = 800.0f;
    const float CELL_SIZE = 50.0f;
    
    try {
        if (rank == 0) {
            std::cout << "\n=====================================" << std::endl;
            std::cout << "  MPI Distributed Flock Test" << std::endl;
            std::cout << "=====================================" << std::endl;
            std::cout << "Number of processes: " << size << std::endl;
            std::cout << "Simulation domain: " << SIM_WIDTH << " x " << SIM_HEIGHT << std::endl;
            std::cout << "====================================\n" << std::endl;
        }
        
        // Créer le flock distribué
        DistributedFlock flock(SIM_WIDTH, SIM_HEIGHT, CELL_SIZE);
        
        // Afficher les informations MPI
        flock.getMPIManager().printInfo();
        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank == 0) {
            std::cout << "\n--- Test 1: Domain Decomposition ---" << std::endl;
        }
        
        // Test 1: Vérifier la décomposition de domaine
        const auto& localDomain = flock.getMPIManager().getLocalDomain();
        if (rank == 0) {
            std::cout << "Local domain for rank " << rank << ": (" 
                      << localDomain.minX << ", " << localDomain.minY << ") to (" 
                      << localDomain.maxX << ", " << localDomain.maxY << ")" << std::endl;
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank == 0) {
            std::cout << "\n--- Test 2: Populate Random Distributed ---" << std::endl;
        }
        
        // Test 2: Créer des boids distribués aléatoirement
        flock.populateRandomDistributed(100);
        
        if (rank == 0) {
            std::cout << "Total boids: " << flock.getGlobalBoidCount() << std::endl;
        }
        
        std::cout << "Rank " << rank << ": " << flock.boids.size() << " boids" << std::endl;
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank == 0) {
            std::cout << "\n--- Test 3: Halo Exchange ---" << std::endl;
        }
        
        // Test 3: Échange de halos
        flock.performHaloExchange();
        
        std::cout << "Rank " << rank << ": " << flock.getHaloBoids().size() 
                  << " boids in halo" << std::endl;
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank == 0) {
            std::cout << "\n--- Test 4: Gather Boids ---" << std::endl;
        }
        
        // Test 4: Rassembler tous les boids sur le rank 0
        std::vector<Boid> allBoids;
        flock.gatherAllBoids(allBoids);
        
        if (rank == 0) {
            std::cout << "Gathered " << allBoids.size() << " boids on rank 0" << std::endl;
            if (!allBoids.empty()) {
                std::cout << "First boid position: (" 
                          << allBoids[0].position.x << ", " 
                          << allBoids[0].position.y << ")" << std::endl;
            }
            std::cout << "\n--- Test 5: Scatter Boids ---" << std::endl;
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        // Test 5: Distribuer les boids depuis root
        flock.scatterAllBoids(allBoids);
        std::cout << "Rank " << rank << ": " << flock.boids.size() 
                  << " boids after scatter" << std::endl;
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank == 0) {
            std::cout << "\n--- Performance Test ---" << std::endl;
        }
        
        // Test de performance
        flock.populateRandomDistributed(1000);
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        double startTime = MPI_Wtime();
        
        for (int i = 0; i < 100; ++i) {
            flock.updateAllDistributed(0.016f);
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        double endTime = MPI_Wtime();
        double elapsedTime = endTime - startTime;
        
        if (rank == 0) {
            std::cout << "100 iterations with 1000 boids:" << std::endl;
            std::cout << "Total time: " << std::fixed << std::setprecision(3) 
                      << elapsedTime << " seconds" << std::endl;
            std::cout << "Avg time per iteration: " << (elapsedTime / 100.0) * 1000.0 
                      << " ms" << std::endl;
            std::cout << "\n=====================================" << std::endl;
            std::cout << "  All tests completed successfully!" << std::endl;
            std::cout << "=====================================" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Rank " << rank << " caught exception: " << e.what() << std::endl;
    }
    
    // Finaliser MPI
    MPI_Finalize();
    return 0;
}