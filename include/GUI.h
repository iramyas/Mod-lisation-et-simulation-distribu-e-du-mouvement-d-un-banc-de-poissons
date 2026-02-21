#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Slider {
public:
    Slider() = delete;
    
    Slider(const sf::Font& font, const std::string& label, float x, float y,
           float w, float min, float max, float value);
    
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window) const;
    
    float getValue() const;
    void setValue(float v) { value = v; updateHandle(); }
    
private:
    float handleToValue(float hx) const;
    void updateHandle();
    
    sf::Text labelText;
    sf::Text valueText;
    sf::RectangleShape bar;
    sf::RectangleShape handle;
    
    float value = 0.f;
    float minValue = 0.f;
    float maxValue = 1.f;
    bool dragging = false;
};
