#ifndef BOID_H  // ← Change #ifdef en #ifndef
#define BOID_H

#include "vector2D.h"
#include <vector>

using simulation::Vector2D;

class Boid {
public:
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration; 
    
    // Paramètres physiques
    float mass;
    float maxSpeed;  
    float maxForce;
    float perceptionRadius;
    
    // Constructeurs
    Boid();
    Boid(float x, float y);
    Boid(Vector2D pos, Vector2D vel);
    
    // Méthodes de mise à jour
    void update(float deltaTime);  
    void applyForce(const Vector2D& force);  
    
    // Les 3 règles de Reynolds
    Vector2D separate(const std::vector<Boid*>& boids);
    Vector2D align(const std::vector<Boid*>& boids);
    Vector2D cohesion(const std::vector<Boid*>& boids);  
    
    Vector2D seek(const Vector2D& target);
    Vector2D flee(const Vector2D& target);
    
private:
    std::vector<Boid*> getNeighbors(const std::vector<Boid*>& boids);
};

#endif
