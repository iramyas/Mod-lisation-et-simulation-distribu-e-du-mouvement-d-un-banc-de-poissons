#pragma once
#include <cmath>
#include <iostream>

namespace simulation {
    inline constexpr float EPSILON = 1e-6f;
    class Vector2D {
    public:
        float x,y;

        // Constructeur et destructeur

        Vector2D() : x(0.0f), y(0.0f) {} // Initialise à 0 si aucun paramètre
        Vector2D(float x_, float y_) : x(x_), y(y_) {} // Initialise avec les paramètres
        ~Vector2D() = default;

        // Operations arithmétiques (+, -, *, /)

        Vector2D operator+(const Vector2D& vec2) const noexcept { return  Vector2D{ x + vec2.x, y + vec2.y }; }
        Vector2D operator-(const Vector2D& vec2) const noexcept { return  Vector2D{ x - vec2.x, y - vec2.y }; }
        Vector2D operator*(float number) const noexcept { return Vector2D{ number * x, number * y }; }
        friend inline Vector2D operator*(float number, const Vector2D& vec) noexcept { return vec * number; }
        Vector2D operator/(float number) const noexcept { 
            if(std::fabs(number) <= EPSILON) {
                return Vector2D{ 0.0f, 0.0f };
            }
            return Vector2D{ x / number, y / number };
        }

        // Opérande de test

        bool operator==(const Vector2D& vec2) const noexcept { return std::fabs(x - vec2.x) <= EPSILON && std::fabs(y - vec2.y) <= EPSILON; }

        // Opérande affectation

        Vector2D& operator+=(const Vector2D& vec2) noexcept {
            x += vec2.x;
            y += vec2.y;
            return *this;
        }

        Vector2D& operator-=(const Vector2D& vec2) noexcept {
            x -= vec2.x;
            y -= vec2.y;
            return *this;
        }

        Vector2D& operator*=(float scalar) noexcept {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        // Opération de négation
        Vector2D operator-() const noexcept { return Vector2D{ -x, -y }; }

        // Opérations vectorielles de base 

        // Norme
        float magnitude() const noexcept { return std::sqrt(x * x + y * y); }
        // Norme au carrée
        float magnitudeSquared() const noexcept { return x * x + y * y; }
        // Vecteur Normalisé
        Vector2D normalized() const noexcept {
            float mag = magnitude();
            if(mag <= EPSILON) return Vector2D{ 0.0f, 0.0f };
            return Vector2D{ x / mag, y / mag};
        }
        // Dot
        float dot(const Vector2D& vec2) const noexcept { return x * vec2.x + y * vec2.y; } 

        // Distance euclidienne
        float distance(const Vector2D& vec2) const noexcept { return (*this - vec2).magnitude(); }

        // Distance au carrée
        float distanceSquared(const Vector2D& vec2) const noexcept { return (*this - vec2).magnitudeSquared(); }

        // Angle (en radians)
        double angle() const noexcept { return std::atan2(y, x); }

        // Rotation du vecteur par un angle (en radian)
        Vector2D rotated(float theta) const noexcept {
            float cos_a = std::cos(theta);
            float sin_a = std::sin(theta);
            return Vector2D{ x * cos_a - y * sin_a, x * sin_a + y * cos_a};
        }

        // Opérateur chevron << pour sortie debug
        friend inline std::ostream& operator<<(std::ostream& os, const Vector2D& vec){
            os << "(" << vec.x << ", " << vec.y << ")";
            return os;
        }

    };
}
