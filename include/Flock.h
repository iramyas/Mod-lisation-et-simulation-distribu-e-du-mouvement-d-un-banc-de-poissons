#pragma once
#include "boid.h"
#include <vector>

/**
 * @namespace simulation
 * @brief Contient toutes les classes et fonctions liées à la simulation des flocks.
 */
namespace simulation{

    /** 
     * @brief Classe gérant un ensemble de boids et leur organisation spatiale
     * 
     * Cette classe maintient la liste des boids, ainsi qu'une grille spatiale permettant d'optimiser la recherche de voisins dans la simulation.
     * 
    */
    class Flock{

        public:

            float neighborRadius = 30.0f; // rayon de vision

            /**
             * @brief Structure interne représentant une grille spatiale d'accélération
             * 
             * La grille subdivise l'espace en cellules afin de réduire la complexité de recherche des voisins.
             * Chaque cellule contient des pointeurs vers les boids situés dans cette région.
             */
            class SpatialGrid{

                public:
                    float cellSize; // Taille d'une cellule de la grille
                    std::vector<std::vector<Boid*>> cells; // Liste des cellules contenant les pointeurs de boids
                    int gridWidth; // Largeur en nombre de cellules
                    int gridHeight; // Hauteur en nombre de cellules

                    /**
                     * @brief Constructeur de SpatialGrid
                     * @param width Largeur totale de l'espace en unités
                     * @param height Hauteur totale de l'espace en unités
                     * @param cellSize Taille d'une cellule
                     */
                    SpatialGrid(float width, float height, float cellSize);

                    /**
                     * @brief Réinitialise la grille en vidant toutes les cellules
                     */
                    void clear();

                    /**
                     * @brief Ajoute un boid dans la cellule correspondant à sa position
                     * @param boid Pointeur vers le boid à ajouter
                     */
                    void addBoid(Boid* boid);

                    /**
                     * @brief Récupère les boids présents dans les cellules proches d'un boid donné
                     * @param boid Pointeur vers le boid dont on cherche les voisins
                     * @return Liste de pointeurs vers les boids voisins potentiels
                     */
                    std::vector<Boid*> queryNeighbors(const Boid* boid) const;

            };

            std::vector<Boid> boids; // Liste des boids de la simulation
            SpatialGrid grid; // SpatialGrid accélérant la recherche de voisins

            /**
             * @brief Constructeur de Flock
             * @param width Largeur totale de la zone de simulation
             * @param height Hauteur totale de la zone de simulation
             * @param cellSize Taille d'une cellule dans le SpatialGrid
             */
            Flock(float width, float height, float cellSize);

            /**
             * @brief Ajoute un nouveau boid au groupe
             * @param boid Boid à ajouter
             */
            void addBoid(const Boid& boid);

            /**
             * @brief Récupère les voisins d'un boid via le SpatialGrid
             * @param boid Pointeur vers le boid dont on cherche les voisins
             * @return Liste de pointeurs de boids voisins
             */
            std::vector<Boid*> getNeighbors(Boid* boid);

            /**
             * @brief Met à jour tous les boids du flock
             * @param deltaTime Pas de temps de la simulation
             */
            void updateAll(float deltaTime);

            /**
             * @brief 
             */
            void render();

    };



}