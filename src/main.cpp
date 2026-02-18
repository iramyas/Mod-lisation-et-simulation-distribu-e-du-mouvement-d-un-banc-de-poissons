#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include "boid.h"
#include "vector2D.h"
#include "Flock.h"
#include "GUI.h"

int main() {
    // Paramètres fenêtre
    const unsigned int WIDTH  = 1920;
    const unsigned int HEIGHT = 1080;

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Banc de poissons");
    window.setFramerateLimit(60);

    // Création du banc
    const int initialN = 20;

    
    simulation::Flock flock(static_cast<float>(WIDTH), static_cast<float>(HEIGHT), 40.f);

    /*
    for (int i = 0; i < initialN; ++i) {
        float x = static_cast<float>(std::rand() % WIDTH);
        float y = static_cast<float>(std::rand() % HEIGHT);
        simulation::Boid b(x, y);
        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.f * 3.14159f;
        b.velocity = simulation::Vector2D(std::cos(angle) * 50.f, std::sin(angle) * 50.f);
        flock.addBoid(b);
    }
    */

    flock.populateRandom(initialN);

    // UI: contrôles
    sf::Font font;
    if(!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")){
        // police système non trouvée (Mac)
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            // police système non trouvée (Linux)
        }
    } 

    // Panel layout: compact frame wrapping the controls
    const float panelWidth = 260.f;
    const float panelMargin = 12.f;
    const float sliderWidth = panelWidth - 2.f*panelMargin - 40.f; 
    const float sliderX = panelMargin + 8.f;
    const float sliderY0 = panelMargin + 28.f;
    const float sliderSpacing = 48.f;

    const int numControls = 5; // sep, ali, coh, radius, count
    float panelHeight = sliderY0 + (numControls) * sliderSpacing + panelMargin + 20.f; 
    panelHeight = std::clamp(panelHeight, 140.f, 320.f);

    const float panelPosX = 60.f; 
    const float panelPosY = 8.f;
    sf::RectangleShape uiPanel(sf::Vector2f(panelWidth, panelHeight));
    uiPanel.setPosition(panelPosX, panelPosY);
    uiPanel.setFillColor(sf::Color(20, 20, 20, 220));
    uiPanel.setOutlineThickness(2.f);
    uiPanel.setOutlineColor(sf::Color(120, 120, 120));


    // sliders for weights et neighbor radius (reduire les forces de sep, cohesion et allignement a 10 pour eviter les comportement explosives)
    Slider sepSlider(font, "Separation", panelPosX + sliderX, panelPosY + sliderY0 + 0*sliderSpacing, sliderWidth, 0.f, 10.f, 3.5f);
    Slider aliSlider(font, "Alignment",  panelPosX + sliderX, panelPosY + sliderY0 + 1*sliderSpacing, sliderWidth, 0.f, 10.f, 2.5f);
    Slider cohSlider(font, "Cohesion",   panelPosX + sliderX, panelPosY + sliderY0 + 2*sliderSpacing, sliderWidth, 0.f, 10.f, 3.0f);
    Slider radiusSlider(font, "Neighbor radius", panelPosX + sliderX, panelPosY + sliderY0 + 3*sliderSpacing, sliderWidth, 5.f, 200.f, 50.f);

    // Count + apply 
    Slider countSlider(font, "Count", panelPosX + sliderX, panelPosY + sliderY0 + 4*sliderSpacing, sliderWidth, 10.f, 500.f, static_cast<float>(initialN));
    
    sf::RectangleShape applyBtn(sf::Vector2f(80.f, 26.f));
    float applyX = panelPosX + (panelWidth - applyBtn.getSize().x)/2.f; 
    
    float applyY = panelPosY + sliderY0 + 4*sliderSpacing + 12.f;
    applyBtn.setPosition(applyX, applyY);
    applyBtn.setFillColor(sf::Color(70,130,180));
    sf::Text applyText("Apply", font, 11);
    applyText.setFillColor(sf::Color::White);
    applyText.setPosition(applyX + (applyBtn.getSize().x - applyText.getLocalBounds().width)/2.f - 2.f,
                          applyY + (applyBtn.getSize().y - applyText.getCharacterSize())/2.f - 2.f);

    // FPS text
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(10);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(8.f, 8.f);
    float fpsSmoothed = 60.f;
    const float fpsAlpha = 0.10f; // 0.05..0.2 selon la rapidité voulue

    sf::Clock clock;

    while (window.isOpen()) {
        // Événements
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();

            // UI events
            sepSlider.handleEvent(event, window);
            aliSlider.handleEvent(event, window);
            cohSlider.handleEvent(event, window);
            radiusSlider.handleEvent(event, window);
            countSlider.handleEvent(event, window);

            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
                sf::Vector2f p = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                if(applyBtn.getGlobalBounds().contains(p)){
                    int cnt = static_cast<int>(countSlider.getValue());
                    flock.boids.clear();
                    for (int i = 0; i < cnt; ++i) {
                        float x = static_cast<float>(std::rand() % WIDTH);
                        float y = static_cast<float>(std::rand() % HEIGHT);
                        simulation::Boid b(x, y);
                        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.f * 3.14159f;
                        b.velocity = simulation::Vector2D(std::cos(angle) * 50.f,
                                                          std::sin(angle) * 50.f);
                        flock.addBoid(b);
                    }
                }
            }
        }

        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f; // évite les gros sauts

        // Mettre à jour FPS
        {
            float fps = dt > 0.f ? 1.f/dt : 0.f;
            std::ostringstream ss;
            ss << "FPS: " << static_cast<int>(fps + 0.5f);
            fpsText.setString(ss.str());
        }

        // ------------------- Mise à jour via Flock (spatial grid + weighted rules)
        // Synchronise le rayon de voisinage
        flock.neighborRadius = radiusSlider.getValue();

        // Reconstruire la grille spatiale
        flock.grid.clear();
        for (auto &b : flock.boids) flock.grid.addBoid(&b);

        // Appliquer règles pondérées et mettre à jour
        for (auto &b : flock.boids) {
            //le slider "Neighbor radius" pilote à la fois la sélection des voisins
            b.perceptionRadius = flock.neighborRadius;

            auto neighbors = flock.getNeighbors(&b);

            simulation::Vector2D sep = b.separate(neighbors);
            simulation::Vector2D ali = b.align(neighbors);
            simulation::Vector2D coh = b.cohesion(neighbors);

            b.applyForce(sep * sepSlider.getValue());
            b.applyForce(ali * aliSlider.getValue());
            b.applyForce(coh * cohSlider.getValue());

            //wrap‑around
            if (b.position.x < 0)        b.position.x += WIDTH;
            if (b.position.x > WIDTH)    b.position.x -= WIDTH;
            if (b.position.y < 0)        b.position.y += HEIGHT;
            if (b.position.y > HEIGHT)   b.position.y -= HEIGHT;

            b.update(dt);  // met à jour position/vitesse 
        }

        // Rendu
        window.clear(sf::Color::Black);
        for (auto &b : flock.boids) {
            // Dessin boid (triangle orienté) 
            sf::ConvexShape fish;
            fish.setPointCount(3);
            fish.setPoint(0, sf::Vector2f(0.f, 0.f));      // nez
            fish.setPoint(1, sf::Vector2f(-12.f, 5.f));    // queue bas
            fish.setPoint(2, sf::Vector2f(-12.f, -5.f));   // queue haut
            fish.setFillColor(sf::Color(80, 200, 255));

            float angle = std::atan2(b.velocity.y, b.velocity.x) * 180.f / 3.14159f;
#if defined(SFML_VERSION_MAJOR) && (SFML_VERSION_MAJOR >= 3)
            fish.setRotation(sf::degrees(angle));
#else
            fish.setRotation(angle);
#endif
            fish.setPosition(b.position.x, b.position.y);
            window.draw(fish);
        }

        // UI
        window.draw(uiPanel);
        sepSlider.draw(window);
        aliSlider.draw(window);
        cohSlider.draw(window);
        radiusSlider.draw(window);
        countSlider.draw(window);
        window.draw(applyBtn);
        window.draw(applyText);
        window.draw(fpsText);

        window.display();
    }

    return 0;
}