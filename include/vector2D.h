#pragma once
#include <cmath>

namespace simulation {
    inline constexpr float EPSILON = 1e-6f;
    class Vector2D {
    public:
        float x,y;

        // Constructeur et Destructeur
        Vector2D() : x(0.0f), y(0.0f) {} // Initialise à 0 si aucun paramètre
        Vector2D(float x_, float y_) : x(x_), y(y_) {} // Initialise avec les paramètres
        ~Vector2D() = default;

        //operations arithmetiques + - / 
        Vector2D operator+(const Vector2D& vec2) const { return  Vector2D{ x + vec2.x, y + vec2.y }; }
        Vector2D operator-(const Vector2D& vec2) const { return  Vector2D{ x - vec2.x, y - vec2.y }; }

        // Opérande de test
        bool operator==(const Vector2D& vec2) const { return std::fabs(x - vec2.x) <= EPSILON && std::fabs(y - vec2.y) <= EPSILON; }

        //operateurs d'affectation +=  

        //methodes vectorielles  ^2 normalized 

        //calculer l'angle


    };
}
