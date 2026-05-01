#include "DistributedFlock.h"
#include <random>

namespace simulation {

/// @brief Constructeur pour simulation distribuée avec MPI
/// 
/// Initialise une Flock avec gestion MPI:
/// - Appelle le constructeur parent Flock
/// - Crée le gestionnaire MPI avec décomposition 2D du domaine
/// - Initialise le compteur global de boids à 0
/// 
/// @param width Largeur totale de la simulation
/// @param height Hauteur totale de la simulation
/// @param cellSize Taille d'une cellule du SpatialGrid pour l'accélération voisinage
/// 
/// @note Chaque processus reçoit un sous-domaine équilibré automatiquement
/// @note Le domaine local est calculé par MPIManager basé sur le rang du processus
DistributedFlock::DistributedFlock(float width, float height, float cellSize)
    : Flock(width, height, cellSize),
      mpiManager(width, height), 
      globalBoidCount(0) {
}

/// @brief Effectue l'échange des zones halo avec les 8 processus voisins
/// 
/// Les boids situés près des frontières du domaine local sont envoyés
/// aux processus voisins et vice-versa. Les boids reçus sont stockés
/// dans haloBoids pour être utilisés dans le calcul des forces.
/// 
/// Étapes internes (MPIManager::haloExchange):
/// 1. Pour chaque voisin (8 directions avec wrap toroïdal)
/// 2. Sélectionner les boids proches de la frontière
/// 3. Échange bidirectionnel MPI_Sendrecv
/// 4. Accumulation dans haloBoids
/// 
/// @note Les haloBoids ne sont JAMAIS modifiés localement (lecture seule)
/// @note Utilisés uniquement pour le calcul des forces (accélération)
/// @note MPI_Sendrecv évite les deadlocks (communication bloquante bidirectionnelle)
/// @see performMigration() qui gère les boids qui quittent le domaine
void DistributedFlock::performHaloExchange() {
    mpiManager.haloExchange(boids, neighborRadius, haloBoids);
}

/// @brief Effectue la migration des boids entre domaines via MPI_Alltoallv
/// 
/// Les boids qui ont quitté le domaine local (suite à updateAll) sont
/// classifiés et envoyés au processus propriétaire du domaine cible.
/// Les boids entrants d'autres processus sont reçus et intégrés.
/// 
/// Après cette opération:
/// - Les boids locaux restent dans la liste `boids`
/// - Les boids entrants sont ajoutés à la fin de `boids`
/// - La liste complète (local + incoming) est prête pour le prochain update
/// 
/// @note Basé sur MPI_Alltoallv pour communication efficace all-to-all
/// @note Les boids restants et entrants sont disjoints (pas de doublons)
/// @see performHaloExchange() qui prépare les données pour le calcul des forces
void DistributedFlock::performMigration() {
    std::vector<Boid> remaining, incoming;
    mpiManager.migrateBoidsAllToAll(boids, remaining, incoming);
    
    // Remplacer les boids locaux par le sous-ensemble qui reste
    boids = remaining;
    
    // Ajouter les boids entrants des autres processus
    boids.insert(boids.end(), incoming.begin(), incoming.end());
}

/// @brief Synchronise le comptage global de boids via MPI_Allreduce(SUM)
/// 
/// Somme le nombre de boids local de tous les processus.
/// Utile pour vérifier la conservation du nombre total de boids
/// (validation que personne ne crée/supprime de boids accidentellement).
/// 
/// Résultat stocké dans globalBoidCount, accessible via getGlobalBoidCount()
/// 
/// @note MPI_Allreduce avec opérateur SUM
/// @note Tous les processus reçoivent le même résultat global
void DistributedFlock::syncGlobalBoidCount() {
    globalBoidCount = mpiManager.syncGlobalCount(boids.size());
}

/// @brief Rassemble tous les boids depuis tous les processus vers le rank 0 (MPI_Gatherv)
/// 
/// Centralise tous les boids sur le processus rank 0 pour inspection,
/// sauvegarde ou comparaison globale.
/// 
/// @param globalBoids [OUT] (rank 0 seulement) Tous les boids de la simulation fusionnés
/// 
/// @note Seul le rank 0 a une globalBoids valide et complète
/// @note Les autres ranks ont une globalBoids vide ou indéfinie
/// @note Utilisé pour debug et tests d'intégrité
void DistributedFlock::gatherAllBoids(std::vector<Boid>& globalBoids) {
    mpiManager.gatherBoids(boids, globalBoids);
}

/// @brief Distribue les boids depuis le rank 0 à tous les processus (MPI_Scatterv)
/// 
/// Divise la liste globale de boids en parts égales et les envoie
/// à chaque processus. Chaque processus reçoit globalBoids.size() / numProcessus boids.
/// 
/// @param globalBoids Liste de boids source (rank 0 seulement; ignorée par autres ranks)
/// 
/// @note Utilisé pour initialiser une simulation distribuée depuis un état global
void DistributedFlock::scatterAllBoids(const std::vector<Boid>& globalBoids) {
    mpiManager.scatterBoids(globalBoids, boids);
}

/// @brief Remplit aléatoirement le domaine local avec des boids
/// 
/// Chaque processus crée (totalN / numProcesses) boids aléatoires
/// dans son domaine spatial local. La graine du générateur aléatoire
/// est décalée par le rang pour éviter les duplicatas.
/// 
/// Résultat:
/// - Distribution complète de totalN boids sur tous les processus
/// - Chaque boid est créé DANS le domaine local du processus
/// - Pas de migration supplémentaire nécessaire (boids déjà chez eux)
/// 
/// @param totalN Nombre total de boids à créer dans toute la simulation
/// 
/// @note Positions: uniformément aléatoires dans [minX, maxX) x [minY, maxY)
/// @note Chaque rank utilise une graine unique (rd() + rank)
/// @note Les vitesses et positions sont initialisées à (0, 0) par Boid()
void DistributedFlock::populateRandomDistributed(int totalN) {
    // Récupérer le rang du processus et le nombre de processus
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Nombre de boids local = totalN réparti équitablement
    int localN = totalN / mpiManager.getSize();
    
    // Récupérer le domaine spatial local
    const auto& domain = mpiManager.getLocalDomain();
    
    // Créer un générateur aléatoire avec graine unique pour ce processus
    std::random_device rd;
    std::mt19937 gen(rd() + rank);
    
    // Distribution uniforme dans les limites du domaine local
    std::uniform_real_distribution<> disX(domain.minX, domain.maxX);
    std::uniform_real_distribution<> disY(domain.minY, domain.maxY);
    
    // Créer localN boids aléatoires
    for (int i = 0; i < localN; ++i) {
        float x = disX(gen);
        float y = disY(gen);
        Boid b(x, y);
        float angle = std::uniform_real_distribution<float>(0.f, 2.f * 3.14159f)(gen);
        b.velocity = Vector2D(std::cos(angle) * 50.f, std::sin(angle) * 50.f);
        boids.emplace_back(b);
    }
}

/// @brief Met à jour tous les boids locaux avec orchestration MPI complète
/// 
/// Étapes:
/// 1. **Halo Exchange** : Communiquer les boids frontière avec voisins (MPI_Sendrecv 8-way)
/// 2. **Grid Rebuild** : Reconstruire SpatialGrid avec boids locaux + halo (accélération voisinage)
/// 3. **Force Computation** : Calculer les forces (utilisant les boids locaux + halo en lecture seule)
/// 4. **Position Integration** : Intégrer les positions (Euler, utilisant Kokkos parallel_for)
/// 5. **Migration** : Les boids qui ont quitté le domaine local sont envoyés ailleurs (MPI_Alltoallv)
/// 6. **Global Synchronization** : Réduire le comptage global (MPI_Allreduce SUM)
/// 
/// Résultat:
/// - Tous les boids locaux ont été mis à jour
/// - Les boids qui se sont déplacés hors du domaine ont été migrés
/// - Le compteur global reflète l'état exact
/// 
/// @param deltaTime Pas de temps pour l'intégration (non utilisé dans cette implémentation)
/// 
/// @note deltaTime peut ne pas être utilisé si le calcul des forces est indépendant du temps
/// @note Les forces et positions sont mises à jour via les méthodes de Flock parent
/// @note Tous les appels MPI sont synchronisés (barrières implicites)
void DistributedFlock::updateAllDistributed(float deltaTime) {
    // 1. Échange des zones halo avec les voisins
    performHaloExchange();
    
    // 2. Reconstruire la grille spatiale (avec boids locaux + halo)
    rebuildSpatialGrid();
    
    // 3. Calcul des forces (Kokkos) — MANQUAIT ENTIÈREMENT
    int numBoids = static_cast<int>(boids.size());

    Kokkos::parallel_for("ComputeFlockForces_Distributed",
        Kokkos::RangePolicy<>(0, numBoids),
        [this](const int i) {
            Boid& b = boids[i];
            b.perceptionRadius = neighborRadius;
            auto neighbors = getNeighbors(&b);  // inclut les boids halo via la grille
            b.applyForce(b.separate(neighbors) * sepWeight);
            b.applyForce(b.align(neighbors)    * aliWeight);
            b.applyForce(b.cohesion(neighbors) * cohWeight);
        }
    );
    Kokkos::fence();

    // 4. Intégration des positions (Kokkos)
    float worldW = grid.gridWidth  * grid.cellSize;
    float worldH = grid.gridHeight * grid.cellSize;

    Kokkos::parallel_for("IntegratePositions_Distributed",
        Kokkos::RangePolicy<>(0, numBoids),
        [this, deltaTime, worldW, worldH](const int i) {
            boids[i].update(deltaTime);
            boids[i].wrapAround(worldW, worldH);
        }
    );
    Kokkos::fence();

    // 5. Effectuer la migration des boids sortis du domaine local
    performMigration();
    
    // 6. Synchroniser le comptage global
    syncGlobalBoidCount();
}

/// @brief Reconstruit la grille spatiale avec les données locales + halo
/// 
/// Vide la grille existante et la repeupl avec:
/// - Les boids locaux (propriété du domaine courant)
/// - Les boids halo (reçus des voisins, lecture seule)
/// 
/// La grille est utilisée pour accélérer les requêtes de voisinage
/// lors du calcul des forces (O(1) en moyenne au lieu de O(n)).
/// 
/// @note Les boids halo restent en lecture seule (pas modifiés localement)
/// @note Les boids locaux seront modifiés lors du calcul des forces
/// @note À appeler APRÈS performHaloExchange() et AVANT calcul des forces
void DistributedFlock::rebuildSpatialGrid() {
    grid.clear();
    
    // Ajouter les boids locaux à la grille
    for (Boid& b : boids) {
        grid.addBoid(&b);
    }
    
    // Ajouter les boids halo (en lecture seule)
    for (Boid& b : haloBoids) {
        grid.addBoid(&b);
    }
}

} // namespace simulation
