#pragma once

#include <mpi.h>
#include <vector>
#include <array>
#include <iostream>

namespace simulation {


struct Boid;

/**
 * @brief Structure représentant le domaine spatial d'un processus
 */
struct Domain {
    float minX, maxX;
    float minY, maxY;
    int procX, procY;

    bool contains(const Boid& boid) const;
    bool inHalo(const Boid& boid, float haloWidth) const;
};

/**
 * @brief Gestionnaire pour la parallélisation distribuée avec MPI
 */
class MPIManager {
public:
    MPIManager() = default; 
    MPIManager(float simWidth, float simHeight);
    ~MPIManager() = default;

    // Halo exchange 
    void haloExchange(const std::vector<Boid>& localBoids,
                     float neighborRadius,
                     std::vector<Boid>& haloBoids);

    // Migrate boids
    void migrateBoidsAllToAll(const std::vector<Boid>& localBoids,
                             std::vector<Boid>& remaining,
                             std::vector<Boid>& incoming);

    
    int syncGlobalCount(int localCount);

   
    void gatherBoids(const std::vector<Boid>& localBoids,
                    std::vector<Boid>& globalBoids);

    
    void scatterBoids(const std::vector<Boid>& globalBoids,
                     std::vector<Boid>& localBoids);

    /**
     * @brief Rassemble tous les boids de tous les processus (MPI_Allgatherv)
     * 
     * Chaque processus reçoit les boids de TOUS les autres processus.
     * Utile pour les tests et comparaisons.
     * 
     * @param localBoids Boids du processus courant
     * @param allBoids [OUT] Tous les boids fusionnés
     */
    void allGatherBoids(const std::vector<Boid>& localBoids,
                       std::vector<Boid>& allBoids);

    // Getters
    const Domain& getLocalDomain() const { return localDomain; }
    int getRank() const { return rank; }
    int getSize() const { return size; }
    MPI_Comm getCartComm() const { return MPI_COMM_WORLD; }

    // Utility
    void printInfo() const;

private:
    int rank = 0;
    int size = 1;
    
   
    int procGridX = 1;     
    int procGridY = 1;     
    int myProcX = 0;     
    int myProcY = 0;       
    
   
    float simWidth = 0.0f;
    float simHeight = 0.0f;
    
    
    Domain localDomain;
};

} // namespace simulation