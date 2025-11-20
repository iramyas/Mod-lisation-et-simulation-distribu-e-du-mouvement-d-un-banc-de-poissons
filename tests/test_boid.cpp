#include "gtest/gtest.h"
#include <cmath>
#include <vector>
#include "vector2D.h"

#include "boid.h"

using simulation::Vector2D;

//########### Tests Constructeur #############//

TEST(BoidTest, ConstructeurDefault) {
    Boid boid;
    
    // Vérifier que position, velocity et acceleration sont initialisés à 0
    EXPECT_FLOAT_EQ(boid.position.x, 0.0f);
    EXPECT_FLOAT_EQ(boid.position.y, 0.0f);
    EXPECT_FLOAT_EQ(boid.velocity.x, 0.0f);
    EXPECT_FLOAT_EQ(boid.velocity.y, 0.0f);
    
    // Vérifier que les paramètres physiques ont des valeurs par défaut
    EXPECT_GT(boid.mass, 0.0f);
    EXPECT_GT(boid.maxSpeed, 0.0f);
    EXPECT_GT(boid.maxForce, 0.0f);
}

TEST(BoidTest, PositionConstructeur) {
    Boid boid(10.0f, 20.0f);
    
    // Vérifier la position
    EXPECT_FLOAT_EQ(boid.position.x, 10.0f);
    EXPECT_FLOAT_EQ(boid.position.y, 20.0f);
    
    // Vérifier que velocity = 0
    EXPECT_FLOAT_EQ(boid.velocity.x, 0.0f);
    EXPECT_FLOAT_EQ(boid.velocity.y, 0.0f);
}

TEST(BoidTest, PositionVelocityConstructeur) {
    Vector2D pos(5.0f, 10.0f);
    Vector2D vel(2.0f, 4.0f);
    Boid boid(pos, vel);
    
    EXPECT_FLOAT_EQ(boid.position.x, 5.0f);
    EXPECT_FLOAT_EQ(boid.position.y, 10.0f);
    EXPECT_FLOAT_EQ(boid.velocity.x, 2.0f);
    EXPECT_FLOAT_EQ(boid.velocity.y, 4.0f);
}

//########### Tests d'Update Physique #############//

TEST(BoidTest, UpdateWithZeroAcceleration) {
    Boid boid(10.0f, 10.0f);
    boid.velocity = Vector2D(1.0f, 1.0f);  
    boid.acceleration = Vector2D(0.0f, 0.0f);  
    
    Vector2D initialVelocity = boid.velocity;
    Vector2D initialPosition = boid.position;
    
    boid.update(1.0f);  
    
    // Position devrait changer selon velocity
    EXPECT_FLOAT_EQ(boid.position.x, initialPosition.x + initialVelocity.x);
    EXPECT_FLOAT_EQ(boid.position.y, initialPosition.y + initialVelocity.y);
}

TEST(BoidTest, VelocityFromAcceleration) {
    Boid boid(0.0f, 0.0f);
    boid.velocity = Vector2D(0.0f, 0.0f);
    boid.acceleration = Vector2D(1.0f, 1.0f);  
    
    boid.update(1.0f);  
    
    EXPECT_FLOAT_EQ(boid.velocity.x, 1.0f);
    EXPECT_FLOAT_EQ(boid.velocity.y, 1.0f);
}

TEST(BoidTest, UpdatePositionFromVelocity) {
    Boid boid(0.0f, 0.0f);  
    boid.velocity = Vector2D(5.0f, 3.0f);
    boid.acceleration = Vector2D(0.0f, 0.0f);
    boid.maxSpeed= 10.0f;
    boid.update(1.0f);  
    
    EXPECT_FLOAT_EQ(boid.position.x, 5.0f);
    EXPECT_FLOAT_EQ(boid.position.y, 3.0f);
}

TEST(BoidTest, UpdateRespectMaxSpeed) {
    Boid boid(0.0f, 0.0f);  
    boid.maxSpeed = 5.0f;
    boid.velocity = Vector2D(0.0f, 0.0f);
    boid.acceleration = Vector2D(10.0f, 10.0f);
    
    // Appliquer update plusieurs fois pour dépasser maxSpeed
    for (int i = 0; i < 10; ++i) {
        boid.update(1.0f);  // ← Ajout du paramètre deltaTime
    }
    
    // Magnitude de velocity ne devrait pas dépasser maxSpeed
    float speed = boid.velocity.magnitude();
    EXPECT_LE(speed, boid.maxSpeed);
}

TEST(BoidTest, ApplyForceAffectsAcceleration) {
    Boid boid(0.0f, 0.0f);
    boid.mass = 2.0f;
    boid.acceleration = Vector2D(0.0f, 0.0f);
    boid.maxForce= 10.0f;
    
    Vector2D force(4.0f, 2.0f);
    boid.applyForce(force); 
    
    // F = ma, donc a = F/m
    EXPECT_FLOAT_EQ(boid.acceleration.x, 2.0f); // 4.0/2.0
    EXPECT_FLOAT_EQ(boid.acceleration.y, 1.0f); // 2.0/2.0
}

TEST(BoidTest, ApplyForceRespectMaxForce) {
    Boid boid(0.0f, 0.0f);
    boid.maxForce = 1.0f;
    boid.mass = 1.0f;
    boid.acceleration = Vector2D(0.0f, 0.0f);
    
    Vector2D largeForce(100.0f, 100.0f);  
    boid.applyForce(largeForce);  
    
    float accelerationMag = boid.acceleration.magnitude();
    EXPECT_LE(accelerationMag, boid.maxForce / boid.mass);
}


//############## Tests de separation ##############//
TEST(BoidTest, SeparateNoNeighbors) { // sans voisins la force de séparation doit etre nulle
    Boid boid(0.0f, 0.0f); 
    std::vector<Boid*> flock;
    Vector2D force= boid.separate(flock);

    EXPECT_FLOAT_EQ(force.x, 0.0f);
    EXPECT_FLOAT_EQ(force.y, 0.0f);
}

TEST(BoidTest, SeparateOneNeighborOnRight) {//la force de separation le pousse vers la gauche
    Boid boid(0.0f, 0.0f);
    Boid neighbor(1.0f, 0.0f); 

    std::vector<Boid*> flock;
    flock.push_back(&neighbor);
    Vector2D force= boid.separate(flock);
    EXPECT_LT(force.x, 0.0f);
}

/*
TEST(BoidTest, SeparateOneBoidTooClose) {//retourne un vecteur qui pointe a l'opposé
    //
}
TEST(BoidTest, SeparateTowBoidsOppositeDirection) { 

}
TEST(BoidTest, SeparateMultipleBoids) { 

}
TEST(BoidTest, SeparateIgnoreDistance) {}

//############## Tests d'Alignement ##############//
TEST(BoidTest, AligneNoNeighbors) { // sans voisins la force de séparation doit etre nulle
    //to do
}
*/
//############## Tests de Cohésion ##############//

//tests de detection des voisins
TEST(BoidTEST, GetNeighborsEmptyFlock){
    Boid boid(0.0f, 0.0f);
    std::vector<Boid*> emptyFlock;
    std::vector<Boid*> neighbors= boid.getNeighbors(emptyFlock);
    EXPECT_EQ(neighbors.size(), 0); 
}
//edge test ()