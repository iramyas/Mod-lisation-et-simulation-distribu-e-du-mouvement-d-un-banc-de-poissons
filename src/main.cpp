#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "boid.h"
#include "vector2D.h"

int main() {
    const unsigned int WIDTH  = 800;
    const unsigned int HEIGHT = 600;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Banc de poissons");
    window.setFramerateLimit(60);

    // Poids des forces 
    float sepWeight = 3.5f;  // séparation
    float aliWeight = 2.5f;  // alignement
    float cohWeight = 3.0f;  // cohésion

    // Création d'un banc de boids
    std::vector<simulation::Boid> boids;
    const int N = 60;
    for (int i = 0; i < N; ++i) {
        float x = static_cast<float>(std::rand() % WIDTH);
        float y = static_cast<float>(std::rand() % HEIGHT);

        simulation::Boid b(x, y);  // constructeur (position)

        // Vitesse initiale aléatoire
        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.f * 3.14159f;
        b.velocity = simulation::Vector2D(std::cos(angle) * 50.f,
                                          std::sin(angle) * 50.f); // px/s 
        boids.push_back(b);
    }

    sf::Font font;
    

    sf::Clock clock;

    while (window.isOpen()) {
        // ------------------- Événements -------------------
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) sepWeight += 0.1f;
                if (event.key.code == sf::Keyboard::Num2) sepWeight -= 0.1f;
                if (event.key.code == sf::Keyboard::Num3) aliWeight += 0.1f;
                if (event.key.code == sf::Keyboard::Num4) aliWeight -= 0.1f;
                if (event.key.code == sf::Keyboard::Num5) cohWeight += 0.1f;
                if (event.key.code == sf::Keyboard::Num6) cohWeight -= 0.1f;
            }
        }

        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f; // évite les gros sauts

        // ------------------- Mise à jour boids -------------------
        // Construire le vecteur de pointeurs pour les règles
        std::vector<simulation::Boid*> flock;
        flock.reserve(boids.size());
        for (auto &b : boids) flock.push_back(&b);

        for (auto &b : boids) {
            simulation::Vector2D sep = b.separate(flock);
            simulation::Vector2D ali = b.align(flock);
            simulation::Vector2D coh = b.cohesion(flock);

            b.applyForce(sep * sepWeight);
            b.applyForce(ali * aliWeight);
            b.applyForce(coh * cohWeight);

            //wrap‑around "a mettre dans boid.cpp maybe"
            if (b.position.x < 0)        b.position.x += WIDTH;
            if (b.position.x > WIDTH)    b.position.x -= WIDTH;
            if (b.position.y < 0)        b.position.y += HEIGHT;
            if (b.position.y > HEIGHT)   b.position.y -= HEIGHT;

            b.update(dt);  // met à jour position/vitesse 
        }

        // ------------------- Rendu -------------------
        window.clear(sf::Color::Black);

        for (auto &b : boids) {
            // Poisson = triangle orienté selon la vitesse
            sf::ConvexShape fish;
            fish.setPointCount(3);
            fish.setPoint(0, sf::Vector2f(0.f, 0.f));      // nez
            fish.setPoint(1, sf::Vector2f(-12.f, 5.f));    // queue bas
            fish.setPoint(2, sf::Vector2f(-12.f, -5.f));   // queue haut
            fish.setFillColor(sf::Color(80, 200, 255));

            float angle = std::atan2(b.velocity.y, b.velocity.x) * 180.f / 3.14159f;
            fish.setRotation(angle);
            fish.setPosition(b.position.x, b.position.y);
            window.draw(fish);
        }

        window.display();
    }

    return 0;
}
