#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::VideoMode mode({800u, 600u});
    sf::RenderWindow window(mode, "Hello, banc de poisson");

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        window.clear();
        window.display();
    }

    std::cout << "Fenetre SFML FERME AVEC SUCCES.." << std::endl;
    return 0;
}
