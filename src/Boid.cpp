#include "Boid.h"
//#include "gtest/gtest.h"
#include <cmath>
#include <vector>

//#include "vector2D.h"

using simulation::Vector2D; 

//contructeur par default
Boid::Boid()
    : position(),
      velocity(),
      acceleration(),
      maxSpeed(5.0f),
      maxForce(0.5f),
      mass(1.0f) {}
//constructeur avec position
Boid::Boid(float x, float y)
    : position(x,y),
      velocity(),

      acceleration(),
      mass(1.0f),
      maxSpeed(5.0f),
      maxForce(0.5f),
      perceptionRadius(50.0f) {}
//constructeur avec position et velocity
Boid::Boid(Vector2D pos, Vector2D vel)
    : position(pos),
      velocity(vel),
      acceleration(),
      mass(1.0f),
      maxSpeed(5.0f),
      maxForce(0.5f),
      perceptionRadius(50.0f) {}


void Boid::update(float deltaTime){
  velocity += acceleration * deltaTime;       //mettre a jour velocity avec aceleration

  if (velocity.magnitude() > maxSpeed) {velocity = velocity.normalized() *maxSpeed;}    //limiter la vitesse a maxSpeed

  position +=velocity *deltaTime;       //mettre a jour la pos avec velocity
  acceleration = Vector2D(0.0f, 0.0f);    //reinitialisation de l'acceleration a 0
}  //mise a jour de la vitesse avec l'accélération /position

void Boid::applyForce(const Vector2D& force){
  //limiter la force à maxForce
  Vector2D f=force;
  if(f.magnitude() >maxForce) {
    f= f.normalized()* maxForce;
  }

  acceleration += f * (1.0f / mass); 
}

//get neighbors 
std::vector<Boid*> Boid::getNeighbors(const std::vector<Boid*>& boids) {
  return std::vector<Boid*>();
}

Vector2D Boid::separate(const std::vector<Boid*>& boids){
  return Vector2D();
}

Vector2D Boid::align(const std::vector<Boid*>& boids){
  return Vector2D();
}

Vector2D Boid::cohesion(const std::vector<Boid*>& boids){
  return Vector2D();
}

Vector2D Boid::seek(const Vector2D& target) {
    return Vector2D();  // TODO
}
Vector2D Boid::flee(const Vector2D& target) {
    return Vector2D();  // todo
}
//rajouter warp around et rebondir