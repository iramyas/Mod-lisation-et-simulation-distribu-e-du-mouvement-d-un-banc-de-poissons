# Modélisation-et-simulation-distribuée-du-mouvement-d-un-banc-de-poissons
Projet M1 chps.

## Membres du projet


## Installation

## Objectifs

Un problème classique pour aborder la modélisation de systèmes complexes et le calcul parallèle est la simulation du comportement collectif dans la nature. L’étude du mouvement d’un banc de poissons constitue un excellent cas d’application : il s’agit d’un système multi- agents où chaque individu suit des règles locales simples (évitement des collisions, alignement, cohésion) qui produisent un comportement collectif émergent.

Le but est de concevoir et de mettre en œuvre un simulateur distribué reproduisant le mouvement d’un banc de poissons, en s’appuyant sur un modèle d’agents autonomes interagis- sant en temps réel. La simulation devra être codée en C++ ou en Python, et exploitera les principes de modélisation multi-agents et de parallélisation pour garantir de bonnes performances lorsque le nombre de poissons simulés devient important.

## Fonctionnement et déroulement de l'implémentation 

**Classe Vector2D.h**
- Pour gérer les positions et les vitesses.

**Classe Boid.h**
- Cette classe représente un poisson, c'est-à-dire ses attributs essentiels et l'ensemble des voisins.

**Les 3 règles de Reynolds**
- Implémentation des règles de "séparation, alignement et cohésion" dans la Classe Boid.

*Séparation*
- Les boids s'éloignent des voisins trop proches pour éviter les collisions. Chaque boid calcule un vecteur de répulsion basé sur la distance aux voisins dans sa zone de perception.

*Alignement*
- Les boids ajustent leur vitesse pour correspondre à la vitesse moyenne de leurs voisins locaux. Cela crée une synchronisation des mouvements au sein du groupe.

*Cohésion*
- Les boids se dirigent vers le centre de masse (position moyenne) de leurs voisins locaux. Cette règle maintient la cohésion du groupe

**Classe Flock**
- Gérer le comportement d'un ensemble de boids (poissons).

**Boucle de simulation main.cpp**
- On utilisera SFML (Simple and Fast Multimedia Library) pour faire les premières simulations en 2D, on testera sur de petites quantités de poissons en faisant varier les forces sur les boids pour vérifier le comportement.

**Faire des tests** 
- Chaque fonction doit être testée et vérifiée pour avoir une modélisation correcte.

**OpenGL**
- Cet outil sera utilisé pour faire la modélisation en 3D. Il faut se familiariser d'abord avec SFML pour pouvoir continuer.

**d'autre types de comportement**
- Il existe d'autre type de comportement d'un Flock de poissons, par exemple "vortex", on peux esseyer egalement de modeliser cela ou simplement comparer!




