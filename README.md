# Modélisation et simulation distribuée du mouvement d'un banc de poissons

Projet M1 chps - Université de Versailles Saint-Quentin-en-Yvelines

## Membres du projet
- Yohann FRONT-REIGNIER
- Iram MADANI-FOUATIH
- Laurence HUANG
- 

## Description

Un problème classique pour aborder la modélisation de systèmes complexes et le calcul parallèle est la simulation du comportement collectif dans la nature. L’étude du mouvement d’un banc de poissons constitue un excellent cas d’application : il s’agit d’un système multi-agents où chaque individu suit des règles locales simples (évitement des collisions, alignement, cohésion) qui produisent un comportement collectif émergent.

Le but est de concevoir et de mettre en œuvre un simulateur distribué reproduisant le mouvement d’un banc de poissons, en s’appuyant sur un modèle d’agents autonomes interagissant en temps réel. La simulation sera codée en C++, et exploitera les principes de modélisation multi-agents et de parallélisation pour garantir de bonnes performances lorsque le nombre de poissons simulés devient important.

**Premier semestre (6 octobre - 19 décembre)**
- Implémentation d'un simulateur séquentiel fonctionnel
- Application des trois règles de Reynolds (séparation, alignement, cohésion)
- Visualisation 2D en temps réel avec SFML
- Validation qualitative du comportement collectif

**Second semestre**
- Parallélisation avec OpenMP
- Parallélisation distribuée avec MPI
- Analyse des performnces et mesures comparatives
- Extensions possibles (obstacles, prédateurs, 3D)

---

## Installation

### Prérequis
- **Système d'exploitation :** Linux et MacOS (non sur Windows)
- **Compilateur :** GCC/Clang avec version C++20
- **CMake :** Version 3.20+

### Dépendances

**Ubuntu :**
```bash
sudo apt update
sudo apt install \
    libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
    libudev-dev libgl1-mesa-dev libfreetype6-dev libgtest-dev \
    cmake doxygen build-essential
```

### Compilation
```bash
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

## Utilisation

### Lancer la simulation
```bash
./bin/main_simulation.bin
```

### Lancer les tests

**Tous les tests :**
```bash
cd build
ctest --output-on-failure
```

**Tests individuels :**
```bash
./bin/test_vector.bin   # Tests Vector2D 
./bin/test_boid.bin     # Tests Boid
```

**Tests avec verbose  (faciliter le debuggage)**
```bash
ctest --verbose
```

### Générer la documentation
```bash
doxygen Doxyfile
firefox "$(realpath docs/html/index.html)" # Ouvrir dans le navigateur
```

## Fonctionnement et déroulement de l'implémentation 

### **Phase 1 : Fondations** (Terminée)

**Classe `Vector2D.h`**
- Gestion des positions, des vitesses et accélérations
- Opérations : addition, soustraction, normalisation, distance, rotation
- **Tests unitaires** : `tests/test_vector.cpp`
- **Statut** : Terminé (21/11/2025)

**Classe `Boid.h`**
- Représentation d'un poisson avec : position, vitesse, accélération
- Méthodes : `update()`, `applyForce()`, règles de Reynolds
- **Tests unitaires** : `tests/test_boid.cpp`
- **Statut** : Terminé (22/11/2025)

### **Phase 2 : Règles de Reynolds** (Terminée)

- **Séparation** : Les boids s'éloignent des voisins trop proches pour éviter les collisions. Chaque boid calcule un vecteur de répulsion basé sur la distance aux voisins dans sa zone de perception.


- **Alignement** : Les boids ajustent leur vitesse pour correspondre à la vitesse moyenne de leurs voisins locaux. Cela crée une synchronisation des mouvements au sein du groupe.

- **Cohésion** : Les boids se dirigent vers le centre de masse (position moyenne) de leurs voisins locaux. Cette règle maintient la cohésion du groupe

- **Paramètres principaux** : vitesse max : 30.0, force max : 2.5, poids : 1.0, rayon de perception : 50 pixels

- **Test unitaires** : `tests/test_boid.cpp` (on créera sûrement un fichier à part pour améliorer la lisibilité)
- **Statut** : Terminé (22/11/2025)

### **Phase 3 : Gestion collective**(fin novembre)

**Classe `Flock`** : Gérer le comportement d'un ensemble de boids et leurs interactions.

- **Statut** : A faire

### **Phase 4 : Simulation visuelle**(debut décembre)

- **Boucle de simulation** : utilisation de SFML pour l'affichage 2D, tests sur différents paramètres et taille de banc.

- **Statut** : A faire

### **Phase 5 : Préparation des benchmarks** (mi-décembre)
- Mesure des performances et du temps d'exécution, documentatio des résultats et des configurations matérielles.

- **Statut** : A faire

### **Phase 6 : Extensions**
**OpenGL**
- Cet outil sera utilisé pour faire la modélisation en 3D. Il faut se familiariser d'abord avec SFML pour pouvoir continuer.

**Comportement avancés**
- Il existe d'autres type de comportement d'un Flock de poissons, par exemple "vortex", on peut essayer également de modéliser cela ou simplement comparer!

## Références

**Modèle de Boids (Craig Reynolds, 1987)**
- [Article original](https://www.red3d.com/cwr/boids/)
- Reynolds, C. W. (1987). "Flocks, herds and schools: A distributed behavioral model"

**Bibliothèques**
- [SFML Documentation](https://www.sfml-dev.org/documentation/)
- [GoogleTest](https://google.github.io/googletest/)
- [Doxygen](https://www.doxygen.nl/)

## Remarques

- Chaque fonction important est testée pour garantir la robustesse de la modélisation.
- Les extensions et benchmarks seront détaillés dans la documentation et le rapport final

**Dernière mise à jour : 24/11/2025**
