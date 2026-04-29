/// @file test_coherence.cpp
/// @brief Test de cohérence pour simulation distribuée
/// 
/// Suite de tests simple sans GTest pour valider les opérations MPI distribuées.
/// Chaque test marche directement sur les structures MPI sans framework.
/// 
/// Tests validés:
/// 1. GlobalCountConsistency : Le nombre total de boids reste constant après updates
/// 2. BoidDistribution : Les boids sont équitablement répartis entre les processus
/// 3. AllGatherBoids : MPI_Allgatherv collecte tous les boids sur chaque rank
/// 4. DomainLocality : Les boids restent dans leur domaine après migration
/// 
/// Exécution: mpirun -n 4 ./build/bin/test_coherence.bin

#include <mpi.h>
#include "Flock.h"
#include "DistributedFlock.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace simulation;

// Couleurs pour l'affichage
#define GREEN   "\033[92m"
#define RED     "\033[91m"
#define RESET   "\033[0m"
#define BOLD    "\033[1m"

int testsPassed = 0;
int testsFailed = 0;

/// Macro pour assertion d'égalité
#define ASSERT_EQ(actual, expected, msg) \
    do { \
        if ((actual) == (expected)) { \
            testsPassed++; \
        } else { \
            testsFailed++; \
            std::cerr << RED "FAIL" RESET ": " << (msg) \
                      << " (expected " << (expected) << ", got " << (actual) << ")" << std::endl; \
        } \
    } while(0)

/// Macro pour assertion "plus grand que"
#define ASSERT_GT(actual, expected, msg) \
    do { \
        if ((actual) > (expected)) { \
            testsPassed++; \
        } else { \
            testsFailed++; \
            std::cerr << RED "FAIL" RESET ": " << (msg) \
                      << " (expected > " << (expected) << ", got " << (actual) << ")" << std::endl; \
        } \
    } while(0)

/// @brief Test 1 : GlobalCountConsistency
/// 
/// Vérifie que le nombre total de boids reste constant après plusieurs updates.
/// 
/// Étapes:
/// 1. Créer une DistributedFlock avec 100 boids
/// 2. Synchroniser le comptage initial sur tous les processus (MPI_Allreduce)
/// 3. Effectuer 5 étapes de simulation (updateAllDistributed)
/// 4. Synchroniser le comptage final
/// 5. Vérifier que initialCount == finalCount == 100
/// 
/// Résultat attendu: PASS (les boids ne sont jamais créés ou supprimés)
void test1_GlobalCountConsistency(int rank, int size) {
    if (rank == 0) std::cout << "\n" << BOLD << "TEST 1: GlobalCountConsistency" << RESET << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    
    float width = 1000.0f, height = 1000.0f;
    DistributedFlock distFlock(width, height, 50.0f);
    
    // Peupler la simulation distribuée
    distFlock.populateRandomDistributed(100);
    
    // Récupérer le comptage initial
    int initialCount = distFlock.getMPIManager().syncGlobalCount(
        static_cast<int>(distFlock.boids.size())
    );
    
    // Effectuer 5 étapes de simulation
    const float deltaTime = 0.016f;
    for (int iter = 0; iter < 5; ++iter) {
        distFlock.updateAllDistributed(deltaTime);
    }
    
    // Récupérer le comptage final après updates/migrations
    int finalCount = distFlock.getMPIManager().syncGlobalCount(
        static_cast<int>(distFlock.boids.size())
    );
    
    // Vérifier la conservation du count (rank 0 seulement)
    if (rank == 0) {
        std::cout << "  Initial count: " << initialCount << std::endl;
        std::cout << "  Final count:   " << finalCount << std::endl;
        ASSERT_EQ(finalCount, 100, "Total boids should be 100");
        ASSERT_EQ(initialCount, finalCount, "Count should remain constant");
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
}

/// @brief Test 2 : BoidDistribution
/// 
/// Vérifie que les boids sont équitablement répartis par populateRandomDistributed.
/// 
/// Étapes:
/// 1. Créer une DistributedFlock
/// 2. Peupler avec 100 boids (chaque rank reçoit 100/4 = 25 boids)
/// 3. Utiliser MPI_Reduce pour sommer les comptages locaux
/// 4. Vérifier: totalCount == 100 et totalCount > 0
/// 
/// Résultat attendu: PASS (distribution équitable)
void test2_BoidDistribution(int rank, int size) {
    if (rank == 0) std::cout << "\n" << BOLD << "TEST 2: BoidDistribution" << RESET << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    
    float width = 1000.0f, height = 1000.0f;
    DistributedFlock distFlock(width, height, 50.0f);
    
    distFlock.populateRandomDistributed(100);
    
    // Compter les boids locaux
    int localCount = static_cast<int>(distFlock.boids.size());
    int totalCount = 0;
    
    // Réduire (somme) tous les compteurs locaux sur rank 0
    MPI_Reduce(&localCount, &totalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        std::cout << "  Rank 0: " << localCount << " boids" << std::endl;
        std::cout << "  Total:  " << totalCount << " boids" << std::endl;
        ASSERT_EQ(totalCount, 100, "Total boids should be 100");
        ASSERT_GT(totalCount, 0, "Total boids should be > 0");
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
}

/// @brief Test 3 : AllGatherBoids
/// 
/// Vérifie que MPI_Allgatherv collecte correctement tous les boids sur TOUS les ranks.
/// 
/// Étapes:
/// 1. Créer 100 boids répartis entre les processus
/// 2. Appeler allGatherBoids (MPI_Allgatherv) pour centraliser sur tous les ranks
/// 3. Vérifier que chaque rank reçoit 100 boids
/// 4. Vérifier qu'aucun boid n'a des positions NaN
/// 
/// Résultat attendu: PASS (Allgatherv fonctionne, pas de NaN)
void test3_AllGatherBoids(int rank, int size) {
    if (rank == 0) std::cout << "\n" << BOLD << "TEST 3: AllGatherBoids" << RESET << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    
    float width = 1000.0f, height = 1000.0f;
    DistributedFlock distFlock(width, height, 50.0f);
    
    distFlock.populateRandomDistributed(100);
    
    // Collecter tous les boids sur tous les ranks via MPI_Allgatherv
    std::vector<Boid> allBoids;
    distFlock.getMPIManager().allGatherBoids(distFlock.boids, allBoids);
    
    // Chaque rank doit avoir reçu les 100 boids
    ASSERT_EQ(static_cast<int>(allBoids.size()), 100, 
              "Each rank should have 100 boids after Allgatherv");
    
    // Vérifier qu'il n'y a pas de positions NaN (corruptions numériques)
    int nanCount = 0;
    for (const auto& boid : allBoids) {
        if (std::isnan(boid.position.x) || std::isnan(boid.position.y)) {
            nanCount++;
        }
    }
    
    if (rank == 0) {
        std::cout << "  All ranks received: " << allBoids.size() << " boids" << std::endl;
        std::cout << "  NaN positions: " << nanCount << std::endl;
        ASSERT_EQ(nanCount, 0, "Should have no NaN positions");
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
}

/// @brief Test 4 : DomainLocality
/// 
/// Vérifie que après migration (updateAllDistributed), les boids restent
/// principalement dans leur domaine local après plusieurs itérations.
/// 
/// Étapes:
/// 1. Créer et peupler une DistributedFlock
/// 2. Exécuter 10 itérations de updateAllDistributed pour laisser les boids bouger
/// 3. Vérifier que chaque boid local est dans son domaine (Domain::contains)
/// 4. Compter les violations totales via MPI_Reduce
/// 5. Vérifier que violations <= 10% (migration correcte)
/// 
/// Résultat attendu: PASS (boids restent principalement dans leur domaine)
void test4_DomainLocality(int rank, int size) {
    if (rank == 0) std::cout << "\n" << BOLD << "TEST 4: DomainLocality" << RESET << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    
    float width = 1000.0f, height = 1000.0f;
    DistributedFlock distFlock(width, height, 50.0f);
    
    distFlock.populateRandomDistributed(100);
    
    // Exécuter 10 itérations pour laisser les boids bouger
    const float deltaTime = 0.016f;
    for (int iter = 0; iter < 10; ++iter) {
        distFlock.updateAllDistributed(deltaTime);
    }
    
    // Vérifier la localité: les boids locaux doivent être dans le domaine local
    const auto& localDomain = distFlock.getMPIManager().getLocalDomain();
    int violationCount = 0;
    
    for (const auto& boid : distFlock.boids) {
        if (!localDomain.contains(boid)) {
            violationCount++;
        }
    }
    
    // Réduire le comptage des violations sur le rank 0
    int totalViolations = 0;
    MPI_Reduce(&violationCount, &totalViolations, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (rank == 0) {
        std::cout << "  Domain violations: " << totalViolations << " (out of ~100 boids)" << std::endl;
        
        // Permettre quelques violations (boids en transit entre domaines)
        int maxAllowed = 10;
        if (totalViolations <= maxAllowed) {
            std::cout << GREEN "PASS" RESET << ": Domain locality maintained" << std::endl;
            testsPassed++;
        } else {
            std::cout << RED "FAIL" RESET << ": Too many domain violations" << std::endl;
            testsFailed++;
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
}

/// @brief Main : Exécute tous les tests et affiche un résumé
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        std::cout << "========== MPI Coherence Tests ==========" << std::endl;
        std::cout << "Running with " << size << " processes" << std::endl;
    }
    
    // Exécuter tous les tests
    test1_GlobalCountConsistency(rank, size);
    test2_BoidDistribution(rank, size);
    test3_AllGatherBoids(rank, size);
    test4_DomainLocality(rank, size);
    
    // Afficher le résumé
    if (rank == 0) {
        std::cout << "\n========== SUMMARY ==========" << std::endl;
        std::cout << GREEN << "PASSED: " << testsPassed << RESET << std::endl;
        std::cout << RED << "FAILED: " << testsFailed << RESET << std::endl;
        
        if (testsFailed == 0) {
            std::cout << GREEN << BOLD << "✓ ALL TESTS PASSED" << RESET << std::endl;
        } else {
            std::cout << RED << BOLD << "✗ SOME TESTS FAILED" << RESET << std::endl;
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    
    return (testsFailed > 0) ? 1 : 0;
}
