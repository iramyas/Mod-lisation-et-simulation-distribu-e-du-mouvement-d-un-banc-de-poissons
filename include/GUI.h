#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <algorithm>

// Petit composant Slider (UI)
class Slider {
public:
    Slider() = default;
    Slider(const sf::Font& font, const std::string& label, float x, float y, float w, float min, float max, float value);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update();
    void draw(sf::RenderWindow& window) const;
    float getValue() const { return value; }
    void setValue(float v) { value = std::clamp(v, minValue, maxValue); updateHandle(); }

private:
    sf::RectangleShape bar;
    sf::RectangleShape handle;
    sf::Text labelText;
    sf::Text valueText;

    float minValue = 0.f;
    float maxValue = 1.f;
    float value = 0.f;

    bool dragging = false;

    void updateHandle();
    float handleToValue(float hx) const;
};
