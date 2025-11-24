#pragma once
#include "vector2D.h"
#include <vector>

/**
 * @namespace simulation
 * @brief Contient toutes les classes et fonctions liées à la simulation des boids.
 */
namespace simulation
{
    /**
     * @brief Représenation d'un boid (agent autonome du banc de poisson)
     * 
     * Un boid possède une position, une vitesse, une accélération et des paramètres physiques.
     * Il applique les trois règles de Reynolds (séparation, alignement, cohésion) pour simuler le comportement collectif.
     */
    class Boid
    {
    public:
        Vector2D position;      ///< Position actuelle du boid dans l'espace
        Vector2D velocity;      ///< Vitesse actuelle du boid
        Vector2D acceleration;  ///< Accélération du boid

        // parametres physiques
        float mass;             ///< Masse du boid
        float maxSpeed;         ///< Vitesse maximale du boid
        float maxForce;         ///< Force maxiamle appliquée
        float perceptionRadius; ///< Rayon de perception pour détecter les voisins

        /**
         * @brief Constructeur par défaut
         */
        Boid();

        /**
         * @brief Constructeur avec coordonnées initiales
         * @param x Position initiale X
         * @param y Position initiale Y
         */
        Boid(float x, float y);

        /**
         * @brief Constructeur avec position et vitesse initiales
         * @param pos Position initiale
         * @param vel Vitesse initiale
         */
        Boid(Vector2D pos, Vector2D vel);

        /**
         * @brief Met à jour la position et la vitesse du boid en fonction de son accélération
         * @param deltaTime Pas pour le temps de la simulation
         */
        void update(float deltaTime);

        /**
         * @brief Applique une force à l'accélération du boid
         * @param force Vecteur force à appliquer
         */
        void applyForce(const Vector2D &force);

        // les 3 regles de reynolds
        /**
         * @brief Calcule la force de répulsion pour éviter les collisions
         * @param boids Liste des autres boids
         * @return Vecteur de steering pour modifier la direction pour éviter les autres boids
         * @note Limite la force à @ref maxForce
         */
        Vector2D separate(const std::vector<Boid *> &boids);

        /**
         * @brief Calcule la vitesse moyenne des voisins pour s'aligner dessus
         * @param boids Liste des autres boids
         * @return Vecteur de steering pour modifier la direction pour s'aligner avec la vitesse des autres boids
         * @note Limite la force à @ref maxForce
         */
        Vector2D align(const std::vector<Boid *> &boids);

        /**
         * @brief Calcule le centre de la masse des voisins
         * @param boids Liste des autres boids
         * @return Vecteur de steering pour la cohésion
         * @note Utilise la méthode @see seek() pour se diriger vers le centre
         */
        Vector2D cohesion(const std::vector<Boid *> &boids);

        /**
         * @brief Calcule la force pour se diriger vers une cible
         * @param target Position cible
         * @return Vecteur de steering pour atteindre la cible
         * @note Limite la force à @ref maxForce
         */
        Vector2D seek(const Vector2D &target);

        /**
         * @brief Calcule la force pour fuire la cible
         * @param target Position à fuir
         * @return Vecteur de steering opposé à la cible
         */
        Vector2D flee(const Vector2D &target);

        /**
         * @brief Renvoie la liste des voisins dans le rayon de perception
         * @param boids Liste de tous les boids
         * @return Liste des pointeurs vers les boids voisins
         */
        std::vector<Boid *> getNeighbors(const std::vector<Boid *> &boids);

        // TODO: Ajouter la gestion des bords (wraparound/edges)
    };
}
