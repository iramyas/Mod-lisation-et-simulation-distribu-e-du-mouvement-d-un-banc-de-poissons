# Modélisation-et-simulation-distribuée-du-mouvement-d-un-banc-de-poissons
Projet M1 chps.

## Membres du projet
- Yohann FRONT-REIGNIER
- Iram MADANI-FOUATIH
- 
- 

## Installation

### Dépendances
```bash
sudo apt update
sudo apt install \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libfreetype6-dev \
    libgtest-dev \
    cmake \
    build-essential
```

### Compilation
```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .

```
* Le ( -Werror ) dans le CMakeLists bloque la compilation de mon coté.

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

## Objectifs

Un problème classique pour aborder la modélisation de systèmes complexes et le calcul parallèle est la simulation du comportement collectif dans la nature. L’étude du mouvement d’un banc de poissons constitue un excellent cas d’application : il s’agit d’un système multi- agents où chaque individu suit des règles locales simples (évitement des collisions, alignement, cohésion) qui produisent un comportement collectif émergent.

Le but est de concevoir et de mettre en œuvre un simulateur distribué reproduisant le mouvement d’un banc de poissons, en s’appuyant sur un modèle d’agents autonomes interagis- sant en temps réel. La simulation devra être codée en C++ ou en Python, et exploitera les principes de modélisation multi-agents et de parallélisation pour garantir de bonnes performances lorsque le nombre de poissons simulés devient important.

## Fonctionnement et déroulement de l'implémentation 

### **Phase 1 : Fondations** (fin novembre)

**Classe Vector2D.h**
- Gestion des positions, des vitesses et accélérations
- Opérations : addition, soustraction, normalisation, distance, rotation
- **Tests unitaires** : `tests/test_vector.cpp`
- **Statut** : Terminé (21/11/2025)

**Classe Boid.h**
- Représentation d'un poisson avec : position, vitesse, accélération
- Méthodes : `update()`, `applyForce()`, règles de Reynolds
- **Tests unitaires** : `tests/test_boid.cpp`
- **Statut** : terminé (21/11/2025)

### **Phase 2 : Règles de Reynolds** (fin novembre)

**Les 3 règles de Reynolds**
- Implémentation des règles de "séparation, alignement et cohésion" dans la Classe Boid.

**Séparation**
- Les boids s'éloignent des voisins trop proches pour éviter les collisions. Chaque boid calcule un vecteur de répulsion basé sur la distance aux voisins dans sa zone de perception.

**Alignement**
- Les boids ajustent leur vitesse pour correspondre à la vitesse moyenne de leurs voisins locaux. Cela crée une synchronisation des mouvements au sein du groupe.

**Cohésion**
- Les boids se dirigent vers le centre de masse (position moyenne) de leurs voisins locaux. Cette règle maintient la cohésion du groupe

### **Phase 3 : Gestion collective**(fin novembre)

**Classe Flock**
- Gérer le comportement d'un ensemble de boids (poissons).

### **Phase 4 : Simulation visuelle**(debut décembre)

**Boucle de simulation main.cpp**
- On utilisera SFML (Simple and Fast Multimedia Library) pour faire les premières simulations en 2D, on testera sur de petites quantités de poissons en faisant varier les forces sur les boids pour vérifier le comportement.

**Faire des tests** 
- Chaque fonction doit être testée et vérifiée pour avoir une modélisation correcte.


### **Phase 5 : Extensions** (Bonus)
**OpenGL**
- Cet outil sera utilisé pour faire la modélisation en 3D. Il faut se familiariser d'abord avec SFML pour pouvoir continuer.

**d'autre types de comportement**
- Il existe d'autres type de comportement d'un Flock de poissons, par exemple "vortex", on peut essayer également de modéliser cela ou simplement comparer!

## Références

**Modèle de Boids (Craig Reynolds, 1987)**
- [Article original](https://www.red3d.com/cwr/boids/)
- Reynolds, C. W. (1987). "Flocks, herds and schools: A distributed behavioral model"

**Bibliothèques**
- [SFML Documentation](https://www.sfml-dev.org/documentation/)
- [GoogleTest](https://google.github.io/googletest/)
- [Doxygen](https://www.doxygen.nl/)


