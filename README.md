# Modélisation et simulation distribuée du mouvement d'un banc de poissons

Projet M1 chps - Université de Versailles Saint-Quentin-en-Yvelines

## Membres du projet
- Yohann FRONT-REIGNIER
- Iram MADANI-FOUATIH
- Laurence HUANG


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
cmake --build .
```

## Utilisation

### Lancer la simulation
```bash
./bin/main_simulation.bin
```

### Interface interactive (nouveau)
- Des *sliders* sont disponibles dans la fenêtre pour ajuster en temps réel : **Separation**, **Alignment**, **Cohesion**, **Neighbor radius**.
- Un *slider* `Count` permet de choisir le nombre de boids et un bouton **Apply** (clic souris) réinitialise la population.
- Contrôles : glisser-déposer les **sliders** pour ajuster les poids et le rayon ; **Apply** réinitialise la population selon `Count`. Appuyer sur **Échap** ferme la fenêtre.

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
./bin/test_flock.bin    # Tests Flock
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

## Semestre 1 : Partie séquentielle (Terminée)

### Phase 1 : Fondations (Terminée)

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

### Phase 3 : Gestion collective (Terminée)

**Classe `Flock`**
- Gestion d'une collection de boids avec ajout et suppression dynamique
- Calcul des interactions entre tous les boids du groupe
- **Optimisation SpatialGrid** : Partitionnement spatial pour accélérer la recherche de voisins (O(n*k) au lieu de O(n²))
- **Tests unitaires** : `tests/test_flock.cpp`
- **Statut** : Terminé (05/12/2025)

### **Phase 4 : Simulation visuelle**(Terminée)

**Interface SFML complète**
- Boucle de simulation temps réel à 60 FPS (500 boids stables)
- Sliders interactifs : Séparation, Alignement, Cohésion, Rayon de voisinage, Nombre de boids
- Bouton Apply pour réinitialisation dynamique de la population
- Affichage FPS en temps réel
- **Statut** : Terminé (16/12/2025)

### Phase 5 : Benchmarks et optimisations (Terminée)
**Mesure des performances**
- Comparaison algorithme naïf vs SpatialGrid : **gain de x5** (500 boids)
- Benchmarks temps vs rayon de voisinage et temps vs nombre de boids
- Graphique de performance : `figures/bench_time_vs_radius.png` et `figures/bench_time_vs_N.png`
- Script de benchmark automatisé : `experiments/benchmark.cpp`
- **Statut** : Terminé (16/12/2025)

## Semestre 2 : Parallélisation et optimisations (en cours)

### Phase 1 : Analyse et profilage (Janvier 2026)

**Identification des goulots d'étranglement**
- Profilage avec `gprof`, `perf` ou `Valgrind` de la version séquentielle
- Mesure du temps passé dans chaque fonction (calcul voisins, règles, affichage)
- Identation des hot spots : boucle principale, recherche de voisins
- **Objectif** : Déterminer les parties du code à paralléliser en priorité
- **Statut** : A faire

### Phase 2 : Parallélisation OpenMP (Janvier - Février 2026)

**Approche mémoire partagée (multi-threads)**
- Parallélisation de la boucle principale de mise à jour des boids (`#pragma omp parallel for`)
- Parallélisation de la construction du SpatialGrid
- Gestion des race conditions et sections critiques
- **Tests de scalabilité** : Mesurer le speedup avec 2, 4, 8, 16 threads
- **Objectif** : Atteindre 4000-5000 boids @ 60 FPS sur 8 threads
- **Statut** : A faire

### Phase 3 : Parallélisation MPI (Février - Mars 2026)

**Approche mémoire distribuée (multi-processus)**
- Décomposition de domaine : partitionnement spatial de la zone de simulation
- Communication des ghost zones (boids aux frontières entre processus)
- Gestion de la migration des boids entre régions
- Load balancing dynamique pour équilibrer la charge
- **Tests de scalabilité** : Strong scaling et weak scaling sur 2, 4, 8, 16 processus
- **Objectif** : Atteindre 10000+ boids @ 60 FPS en distribué
- **Statut** : A faire

### Phase 4 : Approche hybride (optionnel) (Mars 2026)

**Combinaison OpenMP + MPI**
- MPI entre noeuds + OpenMP au sein de chaque noeud
- Optimisation de la communication inter-processus (communication asynchrone)
- Réduction de l'overhead de communication
- **Statut** : A faire

### Phase 5 : Benchmarks et analyse de performances (Mars - Avril 2026)

**Mesure comparatives**
- **Speedup** : $S_p = \frac{T_{seq}}{T_{par}}$ (séquentiel vs parallèle)
- **Efficacité** : $E_p = \frac{S_p}{p}$ (utilisation des ressources)
- **Strong scaling** : Temps de calcul vs nombre de processeurs (problème fixe)
- **Weak scaling** : Efficacité vs taille du problème (charge par processeur fixe)
- Graphique de scalabilité et tableaux de comparatifs
- **Statut** : A faire

### Phase 6 : Extensions et amélioration du réalisme (Avril 2026)

**Comportements avancés**
- Ajout d'obstacles statiques (éviterment de murs)
- Intégration de prédateurs (comportement de fuite)
- Environnement dynamique (courants, zone d'attraction/répulsion)
- Visualisation 3D avec OpenGL (si temps disponible)
- **Statut** : A faire 

## Livrables finaux (Mai 2026)

- **Code source** : Version séquentielle + version parallèles (OpenMP, MPI, hybride)
- **Rapport final** : Analyse complète des performances, graphiques, conclusions
- **Documentation** : Doxygen complète avec guide d'utilisation
- **Présentation** : Soutenance avec démonstration en direct
- **Benchmarks** : Scripts reproductibles et données brutes

## Références

**Modèle de Boids (Craig Reynolds, 1987)**
- [Article original](https://www.red3d.com/cwr/boids/)
- Reynolds, C. W. (1987). "Flocks, herds and schools: A distributed behavioral model"

**Bibliothèques**
- [SFML Documentation](https://www.sfml-dev.org/documentation/)
- [GoogleTest](https://google.github.io/googletest/)
- [Doxygen](https://www.doxygen.nl/)

**Parallélisation**
- [OpenMP Documentation](https://www.openmp.org/)
- [MPI Standard](https://www.mpi-forum.org/)

## Remarques

- Chaque fonction important est testée pour garantir la robustesse de la modélisation.
- Les extensions et benchmarks seront détaillés dans la documentation et le rapport final

**Dernière mise à jour : 12/01/2026** (Bonne année !)
