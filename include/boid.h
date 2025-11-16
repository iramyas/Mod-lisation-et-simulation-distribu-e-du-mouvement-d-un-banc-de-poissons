//definition de l'agent 
#ifdef BOID_H
#define BOID_H

#include "vector2D.h"
#include <vector>


class Boid {
public:
    Vector2D position;
    Vector2D velocity;
    Vector2D accelération;

    //parametres physiques
    float mass;
    float maxspeed;
    float maxForce;



    //constructeurs
    Boid();
    Boid(float x, float y);
    Boid(Vector2D pos, Vector2D vel);
    

    //methodes de mise a jour
    void update();
    void applyForce();

    //les 3 regles de reynolds
    Vector2D separate();
    Vector2D align();
    Vector2D cohersion();

    Vector2D seek();
    Vector2D flee();

    //gestion de bords
    //wraparound()/edges()

private:
    //prendre en agruments un vecteur de pointeurs vers les objets boid pour trouver et renvoyer la liste des voisins
    std::vector<Boid*> getNeighbors(const std::vector<Boid*>& boids);
};


#endif