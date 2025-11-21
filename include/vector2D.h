#pragma once
#include <cmath>
#include <iostream>

namespace simulation {
    inline constexpr float EPSILON = 1e-6f;

    /**
     * @brief Représentation d'un vecteur en 2 dimension pour simuler des comportements physiques
     * 
     * Cette classe gère les positions, vitesses et accélérations dans un espace 2D.
     * Elle fournit toutes les opérations mathématiques pour la simulation de boids.
     */
    class Vector2D {
    public:
        float x; ///< Coordonnées X du vecteur
        float y; ///< Coordonnées Y du vecteur

        // Constructeur et destructeur
        
        /**
         * @brief Constructeur par défaut (initialisation à 0)
         */
        Vector2D() : x(0.0f), y(0.0f) {}

        /**
         * @brief Constructeur avec coordonnées à sa création
         * @param x_ Coordonnées initiale X
         * @param y_ Coordonnées initiale Y
         */
        Vector2D(float x_, float y_) : x(x_), y(y_) {}

        /**
         * @brief Destructeur par défaut
         */
        ~Vector2D() = default;

        // Operations arithmétiques (+, -, *, /)

        /**
         * @brief Addition de 2 vecteurs
         * @param vec2 Vecteur à ajouter
         * @return Vecteur résultant de l'addition
         */
        Vector2D operator+(const Vector2D& vec2) const noexcept { return  Vector2D{ x + vec2.x, y + vec2.y }; }

        /**
         * @brief Soustraction de 2 vecteurs
         * @param vec2 Vecteur à soustraire
         * @return Vecteur résultant de la soustraction
         */
        Vector2D operator-(const Vector2D& vec2) const noexcept { return  Vector2D{ x - vec2.x, y - vec2.y }; }

        /**
         * @brief Multiplication par un scalaire
         * @param number Coefficient multiplicateur
         * @return Nouveau vecteur multiplié
         */
        Vector2D operator*(float number) const noexcept { return Vector2D{ number * x, number * y }; }

        /**
         * @brief Multiplication par un scalaire à gauche
         * @param number Coefficient multiplicateur
         * @param vec Vecteur à multiplié
         * @return Nouveau vecteur multiplié
         */
        friend inline Vector2D operator*(float number, const Vector2D& vec) noexcept { return vec * number; }

        /**
         * @brief Division par un scalaire
         * @param number Diviseur
         * @return Nouveau vecteur divisé
         * @note Protection contre la division par zéro
         * @note Retourne (0, 0) si division par 0
         */
        Vector2D operator/(float number) const noexcept { 
            if(std::fabs(number) <= EPSILON) {
                return Vector2D{ 0.0f, 0.0f };
            }
            return Vector2D{ x / number, y / number };
        }

        /**
         * @brief Comparaison d'égalité avec tolérance
         * @param vec2 Vecteur à comparer
         * @return true si es vecteurs sont égaux à EPSILON près
         */
        bool operator==(const Vector2D& vec2) const noexcept { return std::fabs(x - vec2.x) <= EPSILON && std::fabs(y - vec2.y) <= EPSILON; }

        /**
         * @brief Addition-assignation (modifie le vecteur)
         * @param vec2 Vecteur à ajouter
         * @return Référence au vecteur modifié
         */
        Vector2D& operator+=(const Vector2D& vec2) noexcept {
            x += vec2.x;
            y += vec2.y;
            return *this;
        }

        /**
         * @brief Soustraction-assignation (modifie le vecteur)
         * @param vec2 Vecteur à soustraire
         * @return Référence au vecteur modifié
         */
        Vector2D& operator-=(const Vector2D& vec2) noexcept {
            x -= vec2.x;
            y -= vec2.y;
            return *this;
        }

        /**
         * @brief Multiplication-assignation (modifie le vecteur)
         * @param scalar Scalaire à multiplié
         * @return Référence au vecteur modifié
         */
        Vector2D& operator*=(float scalar) noexcept {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        /**
         * @brief Négation unaire
         * @return Vecteur opposé
         * @note Inverse la direction
         */
        Vector2D operator-() const noexcept { return Vector2D{ -x, -y }; }

        /**
         * @brief Calcule la norme du vecteur
         * @return La norme du vecteur 2D.
         * @note Utilise sqrt() qui est assez coûteuse. Préférer magnitudeSquared()
         *       si on peut se permettre de travailler avec le carré de la norme. 
         * @see magnitudeSquared() pour comparaison plus rapide
         */
        float magnitude() const noexcept { return std::sqrt(x * x + y * y); }
        
        /**
         * @brief Calcule la norme au carré du vecteur
         * @return Le carré de la norme (x² + y²)
         * @note Moins coûteux que magnitude() pour comparaisons de distances
         */
        float magnitudeSquared() const noexcept { return x * x + y * y; }
        
        /**
         * @brief Retourne un vecteur unitaire dans la même direction
         * @return Vecteur normalisé (norme = 1) ou (0, 0) si division par 0
         * @note Protection contre division par zéro avec ESPILON
         */
        Vector2D normalized() const noexcept {
            float mag = magnitude();
            if(mag <= EPSILON) return Vector2D{ 0.0f, 0.0f };
            return Vector2D{ x / mag, y / mag};
        }

        /**
         * @brief Calcule le produit scalaire avec un autre vecteur
         * @param vec2 Vecteur avec lequel calculer le dot product
         * @return Produit scalaire (x1 * x2 + y1 * y2)
         * @note Retourne 0 si vecteurs perpendiculaires, positif et avec la même direction 
         */
        float dot(const Vector2D& vec2) const noexcept { return x * vec2.x + y * vec2.y; } 

        /**
         * @brief Calcule la distance euclidienne entre 2 vecteurs
         * @param vec2 Vecteur de destination
         * @note Utilise magnitude() qui est assez coûteuse. Préférer distanceSquared()
         *       si on peut se permettre de travailler avec le carré de la norme. 
         * @return Distance en unités flottantes
         * @see distanceSquared() pour comparaison plus rapide
         */
        float distance(const Vector2D& vec2) const noexcept { return (*this - vec2).magnitude(); }

        /**
         * @brief Calcule la distance au carrée entre 2 vecteur
         * @param vec2 Vecteur de destination
         * @return Carré de la distance
         * @note Moins coûteux que distance() pour comparaisons de distances
         */
        float distanceSquared(const Vector2D& vec2) const noexcept { return (*this - vec2).magnitudeSquared(); }

        /**
         * @brief Calculle l'angle du vecteur par rapport à l'axe X
         * @return Angle en radian [-pi, pi]
         * @note Peut provoquer des erreurs de l'ordre de 1e-6f
         */
        double angle() const noexcept { return std::atan2(y, x); }

        /**
         * @brief Rotation du vecteur par un angle donnée
         * @param theta Angle de rotation (en radians)
         * @return Nouveau vecteur tourné
         * @note Préserve la magnitude du vecteur.
         * @note Angle positif = rotation antihoraire
         */
        Vector2D rotated(float theta) const noexcept {
            float cos_a = std::cos(theta);
            float sin_a = std::sin(theta);
            return Vector2D{ x * cos_a - y * sin_a, x * sin_a + y * cos_a};
        }

        /**
         * @brief Opérateur de flux pour affichage de debug
         * @param os Flux de sortie
         * @param vec Vecteur à afficher
         * @return Référence au flux (permet chaînage)
         */
        friend inline std::ostream& operator<<(std::ostream& os, const Vector2D& vec){
            os << "(" << vec.x << ", " << vec.y << ")";
            return os;
        }

    };
}
