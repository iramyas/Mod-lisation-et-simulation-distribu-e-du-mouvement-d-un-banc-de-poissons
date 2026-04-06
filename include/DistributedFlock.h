#pragma once

#include "Flock.h"
#include "MPIManager.h"
#include <vector>

namespace simulation {

/**
 * @brief Extension distribuée de Flock utilisant MPI
 * 
 * Cette classe hérite de Flock et ajoute la gestion distribuée des boids:
 * - Chaque processus gère un sous-domaine spatial
 * - Communication des boids aux frontières (halo exchange)
 * - Migration automatique des boids entre domaines
 * - Calcul parallèle des forces avec Kokkos
 */
class DistributedFlock : public Flock {
public:
    
    /**
     * @brief Constructeur pour simulation distribuée
     * @param width Largeur totale de la simulation
     * @param height Hauteur totale de la simulation
     * @param cellSize Taille d'une cellule du SpatialGrid
     */
    DistributedFlock(float width, float height, float cellSize);
    
    /**
     * @brief Destructeur
     */
    virtual ~DistributedFlock() = default;
    
    // ===== Accesseurs =====
    
    /**
     * @return Gestionnaire MPI
     */
    MPIManager& getMPIManager() { return mpiManager; }
    const MPIManager& getMPIManager() const { return mpiManager; }
    
    /**
     * @return Les boids du halo (reçus des voisins)
     */
    const std::vector<Boid>& getHaloBoids() const { return haloBoids; }
    
    /**
     * @return Nombre total de boids dans toute la simulation (tous les processus)
     */
    int getGlobalBoidCount() const { return globalBoidCount; }
    
    // ===== Opérations distribuées =====
    
    /**
     * @brief Remplit aléatoirement le domaine local avec des boids
     * 
     * Les boids sont distribués uniformément sur tous les processus
     * en fonction de leur domaine local.
     * 
     * @param totalN Nombre total de boids à créer dans toute la simulation
     */
    void populateRandomDistributed(int totalN);
    
    /**
     * @brief Met à jour tous les boids locaux avec communication MPI
     * 
     * Étapes:
     * 1. Échange de halos avec les processus voisins
     * 2. Calcul des forces locales (utilisant les boids locaux + halo)
     * 3. Mise à jour des positions des boids locaux
     * 4. Migration des boids qui ont quitté le domaine local
     * 5. Synchronisation globale
     * 
     * @param deltaTime Pas de temps de la simulation
     */
    void updateAllDistributed(float deltaTime);
    
    /**
     * @brief Effectue l'échange de halos avec les voisins
     * 
     * Les boids proches des frontières sont envoyés aux processus voisins.
     * Les boids reçus sont stockés dans haloBoids.
     */
    void performHaloExchange();
    
    /**
     * @brief Effectue la migration des boids entre domaines
     * 
     * Les boids qui ont quitté le domaine local sont envoyés au processus propriétaire.
     * Les boids entrants sont reçus des autres processus.
     * Après cette opération, la liste locale ne contient que les boids du domaine local.
     */
    void performMigration();
    
    /**
     * @brief Synchronise le comptage global des boids
     */
    void syncGlobalBoidCount();
    
    /**
     * @brief Rassemble tous les boids sur le processus root (rank 0)
     * 
     * Utile pour la visualisation ou la sauvegarde des résultats.
     * 
     * @param[out] globalBoids Sur root, contient tous les boids; sur autres, vide
     */
    void gatherAllBoids(std::vector<Boid>& globalBoids);
    
    /**
     * @brief Distribue les boids depuis le processus root à tous les autres
     * 
     * Utile pour charger une configuration initiale.
     * 
     * @param globalBoids Sur root, tous les boids; sur autres, ignoré
     */
    void scatterAllBoids(const std::vector<Boid>& globalBoids);
    
private:
    
    MPIManager mpiManager;           ///< Gestionnaire MPI pour la communication
    std::vector<Boid> haloBoids;     ///< Boids du halo (provenant des voisins)
    int globalBoidCount;             ///< Nombre total de boids dans toute la simulation
    
    /**
     * @brief Reconstruit la grille spatiale avec les boids locaux et halo
     */
    void rebuildSpatialGrid();
};

} // namespace simulation
