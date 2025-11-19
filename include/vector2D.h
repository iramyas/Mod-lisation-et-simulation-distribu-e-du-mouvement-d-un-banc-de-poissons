#pragma once
#include <cmath>

namespace simulation {
    inline constexpr float EPSILON = 1e-6f;
    class Vector2D {
    public:
        float x,y;

        // Constructeur et destructeur

        Vector2D() : x(0.0f), y(0.0f) {} // Initialise à 0 si aucun paramètre
        Vector2D(float x_, float y_) : x(x_), y(y_) {} // Initialise avec les paramètres
        ~Vector2D() = default;

        // Operations arithmétiques (+, -, *)

        Vector2D operator+(const Vector2D& vec2) const { return  Vector2D{ x + vec2.x, y + vec2.y }; }
        Vector2D operator-(const Vector2D& vec2) const { return  Vector2D{ x - vec2.x, y - vec2.y }; }
        Vector2D operator*(float number) const { return Vector2D{ number * x, number * y }; }

        // Opérande de test

        bool operator==(const Vector2D& vec2) const { return std::fabs(x - vec2.x) <= EPSILON && std::fabs(y - vec2.y) <= EPSILON; }

        // Opérande affectation

        Vector2D& operator+=(const Vector2D& vec2) {
            x += vec2.x;
            y += vec2.y;
            return *this;
        }

        Vector2D& operator-=(const Vector2D& vec2) {
            x -= vec2.x;
            y -= vec2.y;
            return *this;
        }

        Vector2D& operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        // Opération de négation
        Vector2D operator-() const { return Vector2D{ -x, -y }; }

        // Opérations vectorielles de base 

        // Norme
        float magnitude() const { return std::sqrt(x * x + y * y); }
        // Norme au carrée
        float magnitudeSquared() const { return x * x + y * y; }
        // Vecteur Normalisé
        Vector2D normalized() const {
            float mag = magnitude();
            if(mag <= EPSILON) return Vector2D{ 0.0f, 0.0f };
            return Vector2D{ x / mag, y / mag};
        }
        // Dot
        float dot(const Vector2D& vec2) const { return x * vec2.x + y * vec2.y; } 

        // Distance euclidienne
        float distance(const Vector2D& vec2) const { return (*this - vec2).magnitude(); }

        // Distance au carrée
        float distanceSquared(const Vector2D& vec2) const { return (*this - vec2).magnitudeSquared(); }

        // Angle (en radians)
        double angle() const { return std::atan2(y, x); }

    };
}
