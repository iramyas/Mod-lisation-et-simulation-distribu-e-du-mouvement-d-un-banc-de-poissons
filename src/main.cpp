#include <iostream>
#include <SFML/Graphics.hpp>

int main() {
    // Création de la fenêtre principale
    sf::VideoMode mode({800, 600});
    sf::RenderWindow window(mode, "Banc de poisson");

    // Main loop pour l'affichage
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        window.display();
    }

    return EXIT_SUCCESS;
}
