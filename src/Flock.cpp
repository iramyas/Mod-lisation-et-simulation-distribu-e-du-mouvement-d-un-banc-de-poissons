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
        grid.clear();
        for(Boid &b : boids){ grid.addBoid(&b); }
        for(Boid &b : boids){
            auto neighbors = grid.queryNeighbors(&b);
            // Appliquer règles pondérées
            b.applyForce(b.separate(neighbors) * sepWeight);
            b.applyForce(b.align(neighbors)    * aliWeight);
            b.applyForce(b.cohesion(neighbors) * cohWeight);
            b.update(deltaTime);
        }
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

            float deg = std::atan2(b.velocity.y, b.velocity.x) * 180.f / 3.14159f;
            fish.setRotation(deg); // plain degrees (SFML 2 compatible)
            fish.setPosition(sf::Vector2f(b.position.x, b.position.y));
        window.draw(fish);

        }
    }

}