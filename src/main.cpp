#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <Kokkos_Core.hpp>
#include "boid.h"
#include "vector2D.h"
#include "Flock.h"
#include "GUI.h"

const int WIDTH = 1200;
const int HEIGHT = 800;

int main() {
    // Initialiser Kokkos
    Kokkos::initialize();
    
    {
        sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Banc de poissons");
        window.setFramerateLimit(60);

        simulation::Flock flock(WIDTH, HEIGHT, 50.f);
        sf::Clock clock;

        sf::Font font;
        const char* fontPaths[] = {
            "/System/Library/Fonts/Helvetica.ttc",
            "/System/Library/Fonts/Courier.ttc",
            "/System/Library/Fonts/Times.ttc",
            "/Library/Fonts/Arial Unicode.ttf"
        };
        
        bool fontLoaded = false;
        for (const auto& path : fontPaths) {
            if (font.openFromFile(path)) {
                fontLoaded = true;
                break;
            }
        }
        
        if (!fontLoaded) {
            std::cerr << "Erreur : impossible de charger une police" << std::endl;
            Kokkos::finalize();
            return -1;
        }


        Slider sepSlider(font, "Separation", 10.f, 50.f, 150.f, 0.f, 10.f, 1.f);
        Slider aliSlider(font, "Alignment", 10.f, 100.f, 150.f, 0.f, 10.f, 1.f);
        Slider cohSlider(font, "Cohesion", 10.f, 150.f, 150.f, 0.f, 10.f, 1.f);
        Slider radiusSlider(font, "Neighbor radius", 10.f, 200.f, 150.f, 20.f, 150.f, 50.f);
        Slider countSlider(font, "Count", 10.f, 250.f, 150.f, 10.f, 1000.f, 500.f);

        // 
        sf::RectangleShape uiPanel({200.f, HEIGHT});
        uiPanel.setFillColor(sf::Color(50, 50, 50));
        uiPanel.setPosition({0.f, 0.f});

        
        sf::RectangleShape applyBtn({80.f, 30.f});
        float applyX = 10.f;
        float applyY = 310.f;  
        applyBtn.setPosition({applyX, applyY});
        applyBtn.setFillColor(sf::Color(100, 150, 100));


        sf::Text applyText(font, "");
        applyText.setString("Apply");
        applyText.setCharacterSize(14);
        applyText.setFillColor(sf::Color::White);
        auto bounds = applyText.getLocalBounds();
        applyText.setPosition({applyX + (applyBtn.getSize().x - bounds.size.x) / 2.f - 2.f,
                               applyY + (applyBtn.getSize().y - bounds.size.y) / 2.f - 5.f});

        sf::Text fpsText(font, "");
        fpsText.setCharacterSize(14);
        fpsText.setFillColor(sf::Color::White);
        fpsText.setPosition({8.f, 8.f});  

        flock.populateRandom(100);
        flock.setWeights(sepSlider.getValue(), aliSlider.getValue(), cohSlider.getValue());
        flock.setNeighborRadius(radiusSlider.getValue());

        while (window.isOpen()) {
            while (auto eventOpt = window.pollEvent()) {
                const auto& event = *eventOpt;

                if (event.is<sf::Event::Closed>()) {
                    window.close();
                }
                
                if (event.is<sf::Event::KeyPressed>()) {
                    auto keyEvent = event.getIf<sf::Event::KeyPressed>();
                    if (keyEvent && keyEvent->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
                }

                sepSlider.handleEvent(event, window);
                aliSlider.handleEvent(event, window);
                cohSlider.handleEvent(event, window);
                radiusSlider.handleEvent(event, window);
                countSlider.handleEvent(event, window);

                if (event.is<sf::Event::MouseButtonPressed>()) {
                    auto mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
                    if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                        sf::Vector2f p = window.mapPixelToCoords({mouseEvent->position.x, mouseEvent->position.y});
                        if (applyBtn.getGlobalBounds().contains(p)) {
                            int cnt = static_cast<int>(countSlider.getValue());
                            flock.boids.clear();
                            for (int i = 0; i < cnt; ++i) {
                                float x = static_cast<float>(std::rand() % WIDTH);
                                float y = static_cast<float>(std::rand() % HEIGHT);
                                simulation::Boid b(x, y);
                                float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.f * 3.14159f;
                                b.velocity = simulation::Vector2D(std::cos(angle) * 50.f, std::sin(angle) * 50.f);
                                flock.addBoid(b);
                            }
                        }
                    }
                }
            }

            float dt = clock.restart().asSeconds();
            if (dt > 0.05f) dt = 0.05f;

            flock.setWeights(sepSlider.getValue(), aliSlider.getValue(), cohSlider.getValue());
            flock.setNeighborRadius(radiusSlider.getValue());
            flock.updateAll(dt);

            // FPS 
            static int frameCount = 0;
            static float timeElapsed = 0.f;
            frameCount++;
            timeElapsed += dt;
            if (timeElapsed >= 1.f) {
                std::ostringstream oss;
                oss << "FPS: " << frameCount;
                fpsText.setString(oss.str());
                frameCount = 0;
                timeElapsed = 0.f;
            }

            
            window.clear(sf::Color(20, 20, 20));
            window.draw(uiPanel);

            sepSlider.draw(window);
            aliSlider.draw(window);
            cohSlider.draw(window);
            radiusSlider.draw(window);
            countSlider.draw(window);

            window.draw(applyBtn);
            window.draw(applyText);
            window.draw(fpsText);

            
            for (auto& b : flock.boids) {
                sf::ConvexShape fish;
                fish.setPointCount(3);
                fish.setPoint(0, {0.f, 0.f});
                fish.setPoint(1, {-12.f, 5.f});
                fish.setPoint(2, {-12.f, -5.f});
                fish.setFillColor(sf::Color(80, 200, 255));

                float angle_rad = std::atan2(b.velocity.y, b.velocity.x);
                fish.setRotation(sf::radians(angle_rad));
                fish.setPosition({b.position.x, b.position.y});
                window.draw(fish);
            }

            window.display();
        }
    }

    // Finaliser Kokkos
    Kokkos::finalize();
    
    return 0;
}