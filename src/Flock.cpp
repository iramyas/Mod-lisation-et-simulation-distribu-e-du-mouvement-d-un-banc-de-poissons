#include "Flock.h"
#include <cmath>

namespace simulation{

    /*
        SPATIALGRID
    */

    Flock::SpatialGrid::SpatialGrid(float width, float height, float cellSize)
    :   cellSize(cellSize), // Stocke la taille d'une cellule
        gridWidth(static_cast<int>(width/cellSize)), // Nombre de cellules en largeur
        gridHeight(static_cast<int>(height/cellSize)) // Nombre de cellules en hauteur
    {

        cells.resize(gridWidth*gridHeight); // Initialise toutes les cellules vides

    }

    void Flock::SpatialGrid::clear(){

        for(auto &cell : cells){ // Parcourt toutes les cellules
            cell.clear(); // Vide la liste des boids dans chaque cellule
        }

    }

    void Flock::SpatialGrid::addBoid(Boid* boid){

        int cx = static_cast<int>(boid->position.x / cellSize); // Calcule l'index X de la cellule
        int cy = static_cast<int>(boid->position.y / cellSize); // Calcule l'index Y de la cellule

        if(cx<0 || cy<0 || cx>=gridWidth || cy>=gridHeight) return; // Vérifie si le boid est hors de la grille(si c'est le cas on l'ignore)

        int index = cy * gridWidth + cx; // Convertit (x,y) en index linéaire
        cells[index].push_back(boid); // Ajoute le boid dans la cellule correspondante

    }

    std::vector<Boid*> Flock::SpatialGrid::queryNeighbors(const Boid* boid) const{

        std::vector<Boid*> neighbors; // Liste de voisins à renvoyer

        int cx = static_cast<int>(boid->position.x / cellSize); // Cellule X du boid
        int cy = static_cast<int>(boid->position.y / cellSize); // Cellule Y du boid
        
        // Parcourt les cellules voisines (3x3 autour du boid)
        for(int dy=-1; dy<=1; ++dy){ // Lignes
            for(int dx=-1; dx<=1; ++dx){ // Colonnes

                int nx = cx + dx; // Calcule cellule voisine X
                int ny = cy + dy; // Calcule cellule voisine Y

                if(nx<0 || ny<0 || nx>=gridWidth || ny>=gridHeight) continue; // Ignore les cellules hors limites

                int index = ny * gridWidth + nx; // Index linéaire de la cellule
                const auto &cell = cells[index]; // Récupère la cellule

                // Ajoute tous les boids de la cellule
                for(Boid* b : cell){

                    if(b!=boid) neighbors.push_back(b); // Voisin != lui-même

                }

            }
        }

        return neighbors;  

    }

    /*
        FLOCK
    */

    Flock::Flock(float width, float height, float cellSize)
    :   grid(width, height, cellSize) // Initialise la grille spatiale
    {
    }

    void Flock::addBoid(const Boid& boid){

        boids.push_back(boid); // Ajoute le boid à la liste

    }

    std::vector<Boid*> Flock::getNeighbors(Boid* boid){

        auto candidates = grid.queryNeighbors(boid); // Récupère les voisins potentiels
        std::vector<Boid*> result; 
        result.reserve(candidates.size());

        // Parcourt tous les voisins potentiels
        for(Boid* b : candidates){

            if(b == boid) continue; 

            float dx = b->position.x - boid->position.x; // distance X
            float dy = b->position.y - boid->position.y; // distance Y

            // Vérifie si b est dans le rayon
            if(dx*dx+dy*dy <= neighborRadius * neighborRadius){

                result.push_back(b);

            }

        }

        return result;

    }

    void Flock::updateAll(float deltaTime){

        grid.clear(); // Vide la grille pour la reconstruire

        // Insère tous les boids dans la grille spatiale
        for(Boid &b : boids){

            grid.addBoid(&b);

        }

        // Met à jour les boids
        for(Boid &b : boids){

            auto neighbors = grid.queryNeighbors(&b); // Récupère les voisins

            // Les 3 règles
            b.applyForce(b.separate(neighbors));
            b.applyForce(b.align(neighbors));
            b.applyForce(b.cohesion(neighbors));

            b.update(deltaTime); // Met à jour

        }

    }

    void Flock::render(){
        //TODO
    
    }


}