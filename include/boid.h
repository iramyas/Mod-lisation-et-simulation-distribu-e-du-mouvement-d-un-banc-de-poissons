//definition de l'agent 

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

    //constructeurs
    Boid();
    //

    //methodes de mise a jour


    //les 3 regles de reynolds

    //gestion de bords

private:
    //prendre en agruments un vecteur de pointeurs vers les objets boid pour trouver et renvoyer la liste des voisins

};