#include "gtest/gtest.h"
#include <cmath>
#include <vector>
#include "Boid.h"
#include "vector2D.h"

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


//Tests de la régle de separation 


//test de la regle d'alignement

//test de la regle de cohesion 

//tests de detection des voisins

//edge test ()