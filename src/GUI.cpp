#include "GUI.h"
#include <iomanip>
#include <sstream>

// Implementation du Slider
Slider::Slider(const sf::Font& font, const std::string& label, float x, float y, float w, float min, float max, float value)
{
    minValue = min; maxValue = max; this->value = std::clamp(value, min, max);
    bar.setPosition(x, y);
    bar.setSize(sf::Vector2f(w, 4.f));
    bar.setFillColor(sf::Color(100, 180, 255));
    handle.setSize(sf::Vector2f(8.f, 14.f));
    handle.setOrigin(handle.getSize().x / 2.f, handle.getSize().y / 2.f);
    handle.setFillColor(sf::Color(255, 180, 100));
    labelText.setFont(font);
    labelText.setCharacterSize(14);
    labelText.setString(label);
    labelText.setPosition(x, y - 20.f);
    labelText.setFillColor(sf::Color::White);
    valueText.setFont(font);
    valueText.setCharacterSize(12);
    valueText.setFillColor(sf::Color::White);
    updateHandle();
}

void Slider::updateHandle() {
    float x = bar.getPosition().x;
    float w = bar.getSize().x;
    float t = (value - minValue) / (maxValue - minValue);
    if (!std::isfinite(t)) t = 0.f;
    float hx = x + t * w;
    handle.setPosition(hx, bar.getPosition().y + bar.getSize().y / 2.f);
    std::ostringstream oss;
    if (labelText.getString() == "Count") {
        int ivalue = static_cast<int>(std::round(value));
        oss << ivalue;
    } else {
        oss << std::fixed << std::setprecision(2) << value;
    }
    valueText.setString(oss.str());
    valueText.setPosition(bar.getPosition().x + bar.getSize().x + 8.f,
                          bar.getPosition().y - 6.f);
}

void Slider::handleEvent(const sf::Event& event, const sf::RenderWindow& window){
    if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
        sf::Vector2f p = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
        if(handle.getGlobalBounds().contains(p)) dragging = true;
        else if(bar.getGlobalBounds().contains(p)){
            float hx = p.x;
            value = handleToValue(hx);
            updateHandle();
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left){
        dragging = false;
    }
    else if(event.type == sf::Event::MouseMoved){
        if(dragging){
            sf::Vector2f p = window.mapPixelToCoords({event.mouseMove.x, event.mouseMove.y});
            float hx = p.x;
            value = handleToValue(hx);
            updateHandle();
        }
    }
}

void Slider::update(){ }

void Slider::draw(sf::RenderWindow& window) const{
    window.draw(bar);
    window.draw(handle);
    window.draw(labelText);
    window.draw(valueText);
}

float Slider::handleToValue(float hx) const{
    float x = bar.getPosition().x;
    float w = bar.getSize().x;
    float t = (hx - x) / w;
    t = std::clamp(t, 0.f, 1.f);
    return minValue + t*(maxValue - minValue);
}
