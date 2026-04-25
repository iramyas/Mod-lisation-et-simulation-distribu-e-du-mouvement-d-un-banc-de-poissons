#include "Flock.h"
#include <cmath>
#include <cstdlib>
#ifdef USE_SFML
#include <SFML/Graphics.hpp>
#endif

namespace simulation {

/// @brief Constructeur de la grille spatiale
/// 
/// Crée une grille 2D pour accélérer les recherches de voisins.
/// La grille divise l'espace en cellules carrées de taille cellSize.
/// 
/// @param width Largeur totale de l'espace
/// @param height Hauteur totale de l'espace
/// @param cellSize Taille d'une cellule (détermine la résolution de la grille)
/// 
/// @note Nombre de cellules = (width/cellSize) × (height/cellSize)
/// @note Chaque cellule contient une liste de pointeurs vers les boids
Flock::SpatialGrid::SpatialGrid(float width, float height, float cellSize)
    : cellSize(cellSize),
      gridWidth(static_cast<int>(width / cellSize)),
      gridHeight(static_cast<int>(height / cellSize)) {
    cells.resize(static_cast<std::size_t>(gridWidth) * static_cast<std::size_t>(gridHeight));
}

/// @brief Vide toutes les cellules de la grille
/// 
/// Réinitialise la grille sans la redimensionner.
/// Appelé au début de chaque pas de simulation avant de reconstruire.
void Flock::SpatialGrid::clear() {
    for (auto& cell : cells) {
        cell.clear();
    }
}

/// @brief Ajoute un boid à la grille en fonction de sa position
/// 
/// Place un pointeur vers le boid dans la cellule correspondant à sa position.
/// Si le boid est en dehors de la grille, il est ignoré (pas d'erreur).
/// 
/// @param boid Pointeur vers le boid à ajouter
/// 
/// @note Complexité: O(1) en moyenne
/// @note Les boids en dehors de la grille ne sont pas ajoutés (pas de débordement)
void Flock::SpatialGrid::addBoid(Boid* boid) {
    int cx = static_cast<int>(boid->position.x / cellSize);
    int cy = static_cast<int>(boid->position.y / cellSize);
    if (cx < 0 || cy < 0 || cx >= gridWidth || cy >= gridHeight) return;
    
    std::size_t index = static_cast<std::size_t>(cy) * static_cast<std::size_t>(gridWidth) + static_cast<std::size_t>(cx);
    cells[index].push_back(boid);
}

/// @brief Retourne les voisins d'un boid dans les cellules adjacentes
/// 
/// Query la grille pour les boids situés dans un voisinage 3×3 cellules
/// (cellule centrale + 8 voisines).
/// Utilisé principalement à titre d'exemple; getNeighbors() est préféré.
/// 
/// @param boid Pointeur vers le boid de référence
/// @return Liste des pointeurs vers les boids adjacents (hors du boid lui-même)
/// 
/// @note Complexité: O(m) où m est le nombre de boids dans les 9 cellules
/// @note Ne filtre PAS par distance (voir getNeighbors pour distance réelle)
std::vector<Boid*> Flock::SpatialGrid::queryNeighbors(const Boid* boid) const {
    std::vector<Boid*> neighbors;
    int cx = static_cast<int>(boid->position.x / cellSize);
    int cy = static_cast<int>(boid->position.y / cellSize);
    
    // Itérer sur les 3×3 cellules (cellule courante + 8 voisines)
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int nx = cx + dx;
            int ny = cy + dy;
            if (nx < 0 || ny < 0 || nx >= gridWidth || ny >= gridHeight) continue;
            
            std::size_t index = static_cast<std::size_t>(ny) * static_cast<std::size_t>(gridWidth) + static_cast<std::size_t>(nx);
            const auto& cell = cells[index];
            
            for (Boid* b : cell) {
                if (b != boid) neighbors.push_back(b);
            }
        }
    }
    return neighbors;
}

/// @brief Constructeur de Flock
/// 
/// Initialise la simulation d'un groupe de boids.
/// La grille spatiale est créée avec les paramètres spécifiés.
/// 
/// @param width Largeur de l'espace de simulation
/// @param height Hauteur de l'espace de simulation
/// @param cellSize Taille d'une cellule de la grille spatiale
/// 
/// @note Les équations de Reynolds (séparation, alignement, cohésion) utilisent
///       les poids par défaut définis dans Flock.h
Flock::Flock(float width, float height, float cellSize)
    : grid(width, height, cellSize) {
}

/// @brief Ajoute un boid à la simulation
/// 
/// Insère un boid à la fin de la liste locale.
/// 
/// @param boid Boid à ajouter
void Flock::addBoid(const Boid& boid) {
    boids.push_back(boid);
}

/// @brief Vide complètement la liste de boids
void Flock::clear() {
    boids.clear();
}

/// @brief Remplit la simulation avec N boids aléatoires
/// 
/// Crée N boids à des positions aléatoires dans la grille.
/// Les vitesses sont initialisées avec des angles aléatoires.
/// Cette fonction réinitialise complètement la simulation.
/// 
/// @param N Nombre de boids à créer
void Flock::populateRandom(int N) {
    clear();
    for (int i = 0; i < N; ++i) {
        int rx = std::rand() % grid.gridWidth;
        int ry = std::rand() % grid.gridHeight;
        float x = rx * grid.cellSize;
        float y = ry * grid.cellSize;
        Boid b(x, y);
        
        // Initialiser la vitesse avec angle aléatoire
        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.f * 3.14159f;
        b.velocity = Vector2D(std::cos(angle) * 50.f, std::sin(angle) * 50.f);
        addBoid(b);
    }
}

/// @brief Récupère les voisins d'un boid selon une distance euclidienne
/// 
/// Query la grille spatiale pour trouver les cellules candidates.
/// Filtre ensuite par distance réelle (neighborRadius).
/// Cela accélère significativement la recherche O(m) au lieu de O(n).
/// 
/// Étapes:
/// 1. Calculer la cellule du boid
/// 2. Calculer la plage de cellules à vérifier (déterminée par neighborRadius / cellSize)
/// 3. Parcourir les cellules dans la plage
/// 4. Filtrer exactement par distance euclidienne
/// 
/// @param boid Pointeur vers le boid de référence
/// @return Vector de pointeurs vers les boids dans le rayon neighborRadius
/// 
/// @note Complexité: O(m) où m est le nombre de boids dans les cellules candidates
/// @note Filtre: distance <= neighborRadius
std::vector<Boid*> Flock::getNeighbors(Boid* boid) {
    std::vector<Boid*> result;
    result.reserve(32);  // Pré-allouer pour décoloration de copies

    // Calculer la cellule du boid
    const int cx = static_cast<int>(boid->position.x / grid.cellSize);
    const int cy = static_cast<int>(boid->position.y / grid.cellSize);

    // Calculer la plage de cellules à vérifier
    const int range = static_cast<int>(std::ceil(neighborRadius / grid.cellSize));
    const float r2 = neighborRadius * neighborRadius;

    // Itérer sur le carré de cellules autour du boid
    for (int dy = -range; dy <= range; ++dy) {
        for (int dx = -range; dx <= range; ++dx) {
            const int nx = cx + dx;
            const int ny = cy + dy;
            if (nx < 0 || ny < 0 || nx >= grid.gridWidth || ny >= grid.gridHeight) continue;

            const std::size_t index = static_cast<std::size_t>(ny) * static_cast<std::size_t>(grid.gridWidth)
                                    + static_cast<std::size_t>(nx);
            const auto& cell = grid.cells[index];

            // Vérifier chaque boid dans la cellule
            for (Boid* b_ptr : cell) {
                if (b_ptr == boid) continue;
                Boid& b = *b_ptr;
                float dx = b.position.x - boid->position.x;
                float dy = b.position.y - boid->position.y;
                if (dx * dx + dy * dy < r2) {
                    result.push_back(b_ptr);
                }
            }
        }
    }
    return result;
}

/// @brief Met à jour tous les boids avec parallélisation Kokkos
/// 
/// Exécute un cycle complet de simulation en 3 phases parallèles:
/// 
/// **Phase 1 : Reconstruction de la grille spatiale**
/// - Vide la grille et ajoute tous les boids
/// - Prépare la structure de données pour les requêtes de voisinage
/// 
/// **Phase 2 : Calcul des forces (parallèle Kokkos)**
/// - Utilise Kokkos::parallel_for avec RangePolicy
/// - Chaque itération i calcule les forces du boid i
/// - Applique les 3 règles de Reynolds:
///   - Séparation (repulsion): s'éloigner des voisins trop proches
///   - Alignement: matcher la vélocité moyenne
///   - Cohésion: attirer vers le centre de masse
/// - Synchronise avec Kokkos::fence() avant de passer à l'étape suivante
/// 
/// **Phase 3 : Intégration et wrapping (parallèle Kokkos)**
/// - Intègre les accélérations pour obtenir les nouvelles vitesses
/// - Intègre les vitesses pour obtenir les nouvelles positions
/// - Applique le wrapping toroïdal aux limites du monde
/// - Synchronise avec Kokkos::fence()
/// 
/// @param deltaTime Pas de temps pour l'intégration (en secondes)
/// 
/// @note Kokkos parallélise automatiquement sur tous les cores disponibles
/// @note Kokkos::fence() synchronise tous les threads avant de continuer
/// @note Pas de race condition : chaque boid traite son propre indice
void Flock::updateAll(float deltaTime) {
    // Phase 1 : Reconstruire la grille spatiale pour accélérer les requêtes de voisinage
    grid.clear();
    for (Boid& b : boids) {
        grid.addBoid(&b);
    }

    // Phase 2 : Calcul parallèle des forces avec Kokkos
    int numBoids = static_cast<int>(boids.size());
    
    Kokkos::parallel_for("ComputeFlockForces",
        Kokkos::RangePolicy<>(0, numBoids),
        [this](const int i) {
            Boid& b = boids[i];
            b.perceptionRadius = neighborRadius;
            
            // Récupérer les voisins depuis la grille spatiale
            auto neighbors = getNeighbors(&b);
        
            // Appliquer les 3 règles de Reynolds avec leurs poids respectifs
            b.applyForce(b.separate(neighbors) * sepWeight);   // Séparation
            b.applyForce(b.align(neighbors)    * aliWeight);   // Alignement
            b.applyForce(b.cohesion(neighbors) * cohWeight);   // Cohésion
        }
    );
    
    // Synchroniser tous les threads Kokkos
    Kokkos::fence();

    // Phase 3 : Intégration parallèle des positions et velocités
    Kokkos::parallel_for("UpdateFlockPositions",
        Kokkos::RangePolicy<>(0, numBoids),
        [this, deltaTime](const int i) {
            // Intégrer la vélocité et la position selon deltaTime
            boids[i].update(deltaTime);
            
            // Appliquer le wrapping toroïdal (tore)
            boids[i].wrapAround(grid.gridWidth * grid.cellSize, 
                               grid.gridHeight * grid.cellSize);  
        }
    );
    
    // Synchronisation finale Kokkos
    Kokkos::fence();
}

/// @brief Définit les poids des trois règles de Reynolds
/// 
/// Les poids contrôlent l'influence relative de chaque règle
/// dans le calcul des forces.
/// 
/// @param sep Poids de la séparation (répulsion)
/// @param ali Poids de l'alignement (vélocité)
/// @param coh Poids de la cohésion (attraction)
/// 
/// @note Valeurs conseillées: séparation=1.5, alignement=1.0, cohésion=1.0
void Flock::setWeights(float sep, float ali, float coh) {
    sepWeight = sep;
    aliWeight = ali;
    cohWeight = coh;
}

/// @brief Définit le rayon de perception pour les voisins
/// 
/// Les boids ne considèrent que les voisins situés à une distance
/// inférieure ou égale à neighborRadius.
/// 
/// @param r Rayon de perception en unités du monde
void Flock::setNeighborRadius(float r) {
    neighborRadius = r;
}

/// @brief Affiche les boids à l'écran (SFML)
/// 
/// Dessine chaque boid comme un petit triangle (poisson)
/// orienté selon sa direction de vélocité.
/// 
/// @param window Fenêtre SFML où dessiner
/// 
/// @note Ne fait rien si USE_SFML n'est pas défini
void Flock::render(sf::RenderWindow& window) {
#ifdef USE_SFML
    for (auto& b : boids) {
        sf::ConvexShape fish;
        fish.setPointCount(3);
        fish.setPoint(0, sf::Vector2f(0.f, 0.f));
        fish.setPoint(1, sf::Vector2f(-12.f, 5.f));
        fish.setPoint(2, sf::Vector2f(-12.f, -5.f));
        fish.setFillColor(sf::Color(80, 200, 255));

        float angle_rad = std::atan2(b.velocity.y, b.velocity.x);
        fish.setRotation(angle_rad * 180.0f / M_PI);
        fish.setPosition(sf::Vector2f(b.position.x, b.position.y));
        window.draw(fish);
    }
#endif
}

} // namespace simulation
