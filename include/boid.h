//definition de l'agent 
#ifndef BOID_H
#define BOID_H

#include "vector2D.h"
#include <vector>

using simulation::Vector2D;

//class BoidTest;

class Boid {
public:
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;

    //parametres physiques
    float mass;
    float maxSpeed;
    float maxForce;

    float perceptionRadius;


    //constructeurs
    Boid();
    Boid(float x, float y);
    Boid(Vector2D pos, Vector2D vel);
    

    //methodes de mise a jour
    void update(float deltaTime);  //mise a jour de la vitesse avec l'accélération /position

    void applyForce(const Vector2D& force);    

    //les 3 regles de reynolds
    Vector2D separate(const std::vector<Boid*>& boids);
    Vector2D align(const std::vector<Boid*>& boids);
    Vector2D cohesion(const std::vector<Boid*>& boids);

    Vector2D seek(const Vector2D& target);
    Vector2D flee(const Vector2D& target);

    //gestion de bords
    //wraparound()/edges()


    //friend class BoidTest;
//private:  //pour le test
    //prendre en agruments un vecteur de pointeurs vers les objets boid pour trouver et renvoyer la liste des voisins
    std::vector<Boid*> getNeighbors(const std::vector<Boid*>& boids);
    
};


#endif