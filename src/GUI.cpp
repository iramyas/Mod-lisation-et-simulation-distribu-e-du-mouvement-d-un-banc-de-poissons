#include "GUI.h"
#include <iomanip>
#include <cmath>
#include <sstream>

// Implementation du Slider
Slider::Slider(const sf::Font& font, const std::string& label, float x, float y, float w, float min, float max, float value)
    : labelText(font, ""), valueText(font, ""), 
      bar({w, 8.f}), handle({12.f, 12.f}),
      minValue(min), maxValue(max), value(value)
{
    labelText.setString(label);
    labelText.setCharacterSize(12);
    labelText.setFillColor(sf::Color::White);
    labelText.setPosition({x, y});
    
    bar.setFillColor(sf::Color(100, 100, 100));
    bar.setPosition({x, y + 15.f});
    
    handle.setFillColor(sf::Color(200, 200, 200));
    handle.setOrigin({6.f, 6.f});
    
    valueText.setCharacterSize(10);
    valueText.setFillColor(sf::Color::Yellow);
    
    updateHandle();
}

void Slider::updateHandle() {
    float x = bar.getPosition().x;
    float w = bar.getSize().x;
    float t = (value - minValue) / (maxValue - minValue);
    if (!std::isfinite(t)) t = 0.f;
    t = std::max(0.f, std::min(1.f, t));
    
    float hx = x + t * w;
    handle.setPosition({hx, bar.getPosition().y + 4.f});
    
    
    std::ostringstream oss;
    if (labelText.getString().find("Count") != std::string::npos) {
        int ivalue = static_cast<int>(std::round(value));
        oss << ivalue;
    } else {
        oss << std::fixed << std::setprecision(2) << value;
    }
    valueText.setString(oss.str());
    valueText.setPosition({x + w + 15.f, bar.getPosition().y - 2.f});
}

void Slider::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.is<sf::Event::MouseButtonPressed>()) {
        auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f p = window.mapPixelToCoords({mouseEvent->position.x, mouseEvent->position.y});
            if (handle.getGlobalBounds().contains(p)) {
                dragging = true;
            } else if (bar.getGlobalBounds().contains(p)) {
                float newX = p.x;
                value = handleToValue(newX);
                updateHandle();
                dragging = false;
            }
        }
    } else if (event.is<sf::Event::MouseButtonReleased>()) {
        auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
            dragging = false;
        }
    } else if (event.is<sf::Event::MouseMoved>()) {
        if (dragging) {
            auto* moveEvent = event.getIf<sf::Event::MouseMoved>();
            if (moveEvent) {
                sf::Vector2f p = window.mapPixelToCoords({moveEvent->position.x, moveEvent->position.y});
                float newX = p.x;
                value = handleToValue(newX);
                updateHandle();
            }
        }
    }
}

void Slider::update() {
    //pas besoin
}

void Slider::draw(sf::RenderWindow& window) const {
    window.draw(bar);
    window.draw(handle);
    window.draw(labelText);
    window.draw(valueText);
}

float Slider::handleToValue(float hx) const {
    float x = bar.getPosition().x;
    float w = bar.getSize().x;
    float t = (hx - x) / w;
    t = std::max(0.f, std::min(1.f, t));
    return minValue + t * (maxValue - minValue);
}

float Slider::getValue() const {
    return value;
}
