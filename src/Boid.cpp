#include "Boid.h"
#include <cmath>
#include <iostream>

Boid::Boid()
    : position()
      velocity()
      acceleration()
      maxSpeed()
      maxForce()
      mass() {}

Boid::Boid(float x, float y)
    : position(x,y)
      velocity() {}
      ///


void Boid::update(){}  //mise a jour de la vitesse avec l'accélération /position

void Boid::applyForce(){}

//get neighbors 
std::vector<Boid*> Boid::getNeighbors(const std::vector<Boid*>& boids) {}

Vector2D Boid::separate(){}

Vector2D Boid::align(){}

Vector2D Boid::cohesion(){}

Vector2D Boid::seek(){}
Vector2D Boid::flee(){}

//rajouter warp around et rebondir? 