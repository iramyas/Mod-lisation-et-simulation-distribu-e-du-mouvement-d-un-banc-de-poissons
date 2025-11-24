#include "boid.h"
#include <cmath>
#include <vector>

#include "vector2D.h"


//contructeur par default
simulation::Boid::Boid()
    : position(),
      velocity(),
      acceleration(),
      mass(1.0f),
      maxSpeed(30.0f),
      maxForce(2.5f) {}
//constructeur avec position
simulation::Boid::Boid(float x, float y)
    : position(x,y),
      velocity(),
      acceleration(),
      mass(1.0f),
      maxSpeed(30.0f),
      maxForce(2.5f),
      perceptionRadius(50.0f) {}
//constructeur avec position et velocity
simulation::Boid::Boid(Vector2D pos, Vector2D vel)
    : position(pos),
      velocity(vel),
      acceleration(),
      mass(1.0f),
      maxSpeed(30.0f),
      maxForce(2.5f),
      perceptionRadius(50.0f) {}


void simulation::Boid::update(float deltaTime){
  velocity += acceleration * deltaTime;       //mettre a jour velocity avec aceleration

  if (velocity.magnitude() > maxSpeed) {velocity = velocity.normalized() *maxSpeed;}    //limiter la vitesse a maxSpeed

  position +=velocity *deltaTime;       //mettre a jour la pos avec velocity
  acceleration = Vector2D(0.0f, 0.0f);    //reinitialisation de l'acceleration a 0
}  //mise a jour de la vitesse avec l'accélération /position

void simulation::Boid::applyForce(const Vector2D& force){
  //limiter la force à maxForce
  Vector2D f=force;
  if(f.magnitude() >maxForce) {
    f= f.normalized()* maxForce;
  }

  acceleration += f * (1.0f / mass); 
}



//get neighbors 
std::vector<simulation::Boid*> simulation::Boid::getNeighbors(const std::vector<Boid*>& boids) {
  std::vector<Boid*> neighbors;

  for (Boid* other : boids){
    if (other != this){ //pas lui meme
      float distance = (other->position - position).magnitude();
      if (distance < perceptionRadius) {
        neighbors.push_back(other);
      }
    }
  }
  return neighbors;
}

simulation::Vector2D simulation::Boid::separate(const std::vector<Boid*>& boids){
  Vector2D steering(0.0f, 0.0f);
  int count=0;   //compte le nombre de voisins et sers pour faire la moyenne a la fin
  for (Boid* other :boids){  //boucle sur chaque pointeur other dans le vecteur boids
    if (other == this) continue; 
    Vector2D diff= position -other->position;         //calcule le vecteur diff allant de l'autre boid vers celui ci
    float d= diff.magnitude();      //calcule la distance entre 2 boids 
    if (d> 0.0f && d < perceptionRadius){  //distance strict positive . si  un boid est trop loin ca n'influence pas la separation
      diff= diff / d ;   //il manque l'operateur division par scalaire dans Vector2D
      steering += diff;
      count++;
    }
  }
  if (count > 0){steering = steering /static_cast<float>(count);}//moyenne 
  else { return Vector2D(0.0f, 0.0f);}
  if (steering.magnitude() > 0.0f){
    steering = steering.normalized() *maxSpeed;
    steering -=velocity;
    if (steering.magnitude() > maxForce) {
      steering = steering.normalized() *maxForce;
    }
  }
  return steering;
}



simulation::Vector2D simulation::Boid::align(const std::vector<Boid*>& boids){
  Vector2D steering(0.0f, 0.0f);
  int count=0;
  for (Boid*other :boids) {
    if(other==this) continue;
    float distance =(other->position -position).magnitude();
    if(distance >0.0f &&distance < perceptionRadius){
      steering +=other->velocity;
      ++count; 
    }
  }
  if (count>0) {
    steering= steering /static_cast<float>(count);

    if (steering.magnitude()>0.0f) {
      steering= steering.normalized()*maxSpeed;
    }
    steering -= velocity;     // le boid doit s'ajuster a la meeme vitesse que ses voisins 
    if (steering.magnitude() > maxForce){  // mais ils ont deja la meme vitesse donc 0 diff a part si on ajoute des tailles differentes 
      steering =steering.normalized() * maxForce; 
    }
  }
  return steering;   //retourne le vecteur (force d'alignement)
}




simulation::Vector2D simulation::Boid::cohesion(const std::vector<Boid*>& boids){
  Vector2D center(0.0f,0.0f);
  int count = 0;
   
  for (Boid* other :boids) {
    if (other == this) continue;
    float distance = (other->position - position).magnitude();
    if (distance >0.0f && distance < perceptionRadius) {
      center += other->position;
      ++count;
    }
  }
  if (count==0){
    return Vector2D(0.0f, 0.0f);
  }
  center= center /static_cast<float>(count);

  return seek(center);      //utiliser seek pour aller vers le centre
}



simulation::Vector2D simulation::Boid::seek(const Vector2D& target) {
  Vector2D desired =target - position;

  if (desired.magnitude()==0.0f){
    return Vector2D(0.0f, 0.0f);
  }
  desired = desired.normalized() * maxSpeed;
  Vector2D steering= desired - velocity;

  if (steering.magnitude() > maxForce){
    steering= steering.normalized() * maxForce;
  }
  return steering;
}



simulation::Vector2D simulation::Boid::flee(const Vector2D& target) {
  Vector2D desired = position - target;
  if (desired.magnitude() == 0.0f) {
    return Vector2D(0.0f, 0.0f);
  }
  desired = desired.normalized() * maxSpeed;
  Vector2D steering = desired - velocity;

  if (steering.magnitude() > maxForce) {
    steering = steering.normalized() * maxForce; 
  }
  return steering; 
}
//rajouter warp around et rebondir