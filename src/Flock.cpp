#include "Flock.h"
#include <cmath>
#include <cstdlib>
#include <SFML/Graphics.hpp>

namespace simulation{

    /*
        SPATIALGRID
    */

    Flock::SpatialGrid::SpatialGrid(float width, float height, float cellSize)
    :   cellSize(cellSize), // Stocke la taille d'une cellule
        gridWidth(static_cast<int>(width/cellSize)), // Nombre de cellules en largeur
        gridHeight(static_cast<int>(height/cellSize)) // Nombre de cellules en hauteur
    {

        cells.resize(static_cast<std::size_t>(gridWidth) * static_cast<std::size_t>(gridHeight)); // Initialise toutes les cellules vides

    }

    void Flock::SpatialGrid::clear(){
        for(auto &cell : cells){
            cell.clear();
        }
    }

    void Flock::SpatialGrid::addBoid(Boid* boid){
        int cx = static_cast<int>(boid->position.x / cellSize);
        int cy = static_cast<int>(boid->position.y / cellSize);
        if(cx<0 || cy<0 || cx>=gridWidth || cy>=gridHeight) return;
        std::size_t index = static_cast<std::size_t>(cy) * static_cast<std::size_t>(gridWidth) + static_cast<std::size_t>(cx);
        cells[index].push_back(boid);
    }

    std::vector<Boid*> Flock::SpatialGrid::queryNeighbors(const Boid* boid) const{
        std::vector<Boid*> neighbors;
        int cx = static_cast<int>(boid->position.x / cellSize);
        int cy = static_cast<int>(boid->position.y / cellSize);
        for(int dy=-1; dy<=1; ++dy){
            for(int dx=-1; dx<=1; ++dx){
                int nx = cx + dx;
                int ny = cy + dy;
                if(nx<0 || ny<0 || nx>=gridWidth || ny>=gridHeight) continue;
                std::size_t index = static_cast<std::size_t>(ny) * static_cast<std::size_t>(gridWidth) + static_cast<std::size_t>(nx);
                const auto &cell = cells[index];
                for(Boid* b : cell){
                    if(b!=boid) neighbors.push_back(b);
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

    void Flock::clear(){
        boids.clear();
    }

    void Flock::populateRandom(int N){
        clear();
        for(int i=0;i<N;++i){
            int rx = std::rand() % grid.gridWidth;
            int ry = std::rand() % grid.gridHeight;
            float x = rx * grid.cellSize;
            float y = ry * grid.cellSize;
            Boid b(x, y);
            float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.f * 3.14159f;
            b.velocity = Vector2D(std::cos(angle) * 50.f, std::sin(angle) * 50.f);
            addBoid(b);
        }
    }

    std::vector<Boid*> Flock::getNeighbors(Boid* boid){

        std::vector<Boid*> result;
        result.reserve(32);

        // Parcours des cellules dans un voisinage suffisamment large pour couvrir neighborRadius.
        const int cx = static_cast<int>(boid->position.x / grid.cellSize);
        const int cy = static_cast<int>(boid->position.y / grid.cellSize);

        const int range = static_cast<int>(std::ceil(neighborRadius / grid.cellSize));
        const float r2 = neighborRadius * neighborRadius;

        for(int dy = -range; dy <= range; ++dy){
            for(int dx = -range; dx <= range; ++dx){
                const int nx = cx + dx;
                const int ny = cy + dy;
                if(nx < 0 || ny < 0 || nx >= grid.gridWidth || ny >= grid.gridHeight) continue;

                const std::size_t index = static_cast<std::size_t>(ny) * static_cast<std::size_t>(grid.gridWidth)
                                        + static_cast<std::size_t>(nx);
                const auto &cell = grid.cells[index];

                for (Boid* b_ptr : cell) { 
                    Boid& b = *b_ptr;
                    float dx = b.position.x - boid->position.x;
                    float dy = b.position.y - boid->position.y;
                    if (dx*dx + dy*dy < r2) {
                        result.push_back(b_ptr);
                    }
                }
            }
        }
        return result;

    }

    void Flock::updateAll(float deltaTime){
        // PHASE 1: Reconstruire grille spatiale
        grid.clear();
        for (Boid& b : boids) {
            grid.addBoid(&b);
        }
    
        // PHASE 2: Parallélisation Kokkos - Calcul des forces
        // Utilisation de Kokkos::parallel_for pour paralléliser sur tous les cores disponibles
        int numBoids = static_cast<int>(boids.size());
        
        Kokkos::parallel_for("ComputeFlockForces",
            Kokkos::RangePolicy<>(0, numBoids),
            [this](const int i) {
                Boid& b = boids[i];
                b.perceptionRadius = neighborRadius;
                auto neighbors = getNeighbors(&b);
            
                b.applyForce(b.separate(neighbors) * sepWeight);
                b.applyForce(b.align(neighbors)    * aliWeight);
                b.applyForce(b.cohesion(neighbors) * cohWeight);
            }
        );
        
        // Synchronisation des threads Kokkos
        Kokkos::fence();
    
        // PHASE 3: Parallélisation Kokkos - Mise à jour positions et wrap around
        Kokkos::parallel_for("UpdateFlockPositions",
            Kokkos::RangePolicy<>(0, numBoids),
            [this, deltaTime](const int i) {
                boids[i].update(deltaTime);
                boids[i].wrapAround(grid.gridWidth * grid.cellSize, 
                                   grid.gridHeight * grid.cellSize);  
            }
        );
        
        // Synchronisation finale
        Kokkos::fence();
    }


    void Flock::setWeights(float sep, float ali, float coh){
        sepWeight = sep; aliWeight = ali; cohWeight = coh;
    }

    void Flock::setNeighborRadius(float r){
        neighborRadius = r;
    }


    void Flock::render(sf::RenderWindow &window){
        for(auto &b : boids){
            sf::ConvexShape fish;
            fish.setPointCount(3);
            fish.setPoint(0, sf::Vector2f(0.f, 0.f));
            fish.setPoint(1, sf::Vector2f(-12.f, 5.f));
            fish.setPoint(2, sf::Vector2f(-12.f, -5.f));
            fish.setFillColor(sf::Color(80, 200, 255));

            float angle_rad = std::atan2(b.velocity.y, b.velocity.x);
            fish.setRotation(sf::radians(angle_rad));
            fish.setPosition(sf::Vector2f(b.position.x, b.position.y));
            window.draw(fish);
        }
    }

}