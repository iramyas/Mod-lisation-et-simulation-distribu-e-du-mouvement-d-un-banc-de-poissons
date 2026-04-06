# Documentation de l'implémentation MPI pour la simulation de banc de poissons

## 1. Architecture Générale

L'implémentation distribuée du simulateur de banc de poissons repose sur une architecture de décomposition spatiale du domaine de simulation. Le calcul est géré par deux classes principales :

* **`MPIManager`** : Agit comme le cerveau réseau. Il centralise toute la logique MPI, les communications (Point-to-Point et Collectives), et la topologie de la grille.
* **`DistributedFlock`** : Hérite de la classe `Flock` séquentielle. Elle orchestre la mise à jour des boids en appelant le `MPIManager` pour synchroniser les données avant d'appliquer la logique métier (règles de Reynolds).

## 2. Décomposition du Domaine (Cartésienne 2D)

Au démarrage de la simulation, l'espace 2D global est découpé en une grille cartésienne. Chaque processus MPI devient responsable d'une sous-région rectangulaire (le **Domaine Local**).

* La grille est créée via `MPI_Dims_create(size, 2, dims)`.
* Les coordonnées de chaque processus (`myProcX`, `myProcY`) sont calculées à partir de son `rank`.
* Les limites du domaine (`minX`, `maxX`, `minY`, `maxY`) sont déduites dynamiquement en fonction de la taille globale de la fenêtre (`simWidth`, `simHeight`).

## 3. Échange de Halos (Ghost Cells)

Pour que les boids situés sur les bords d'un sous-domaine puissent appliquer les règles de séparation, d'alignement et de cohésion, ils ont besoin de "voir" les boids des processus voisins. C'est le rôle de l'**échange de halos**.

* **Critère** : Un boid appartient au halo d'un voisin s'il se situe à une distance inférieure au `neighborRadius` de la frontière du sous-domaine.
* **Topologie** : Chaque processus communique avec ses 8 voisins (Nord, Sud, Est, Ouest, et les 4 diagonales).
* **Conditions Périodiques** : L'espace est torique (wraparound). Si un domaine est sur le bord de la fenêtre globale, son voisin logique "reboucle" de l'autre côté de l'écran.
* **Mécanisme** : La fonction `haloExchange` utilise `MPI_Sendrecv` en deux passes : une pour transmettre le nombre de boids à échanger, et une seconde (avec `MPI_BYTE`) pour transférer la charge utile.

## 4. Migration des Agents

Les boids bougeant à chaque frame, ils finissent par sortir de leur domaine local. La méthode `migrateBoidsAllToAll` (*actuellement en cours de développement*) est chargée de :
1. Détecter quels boids ont dépassé les frontières `[minX, maxX]` ou `[minY, maxY]`.
2. Les extraire du tableau `localBoids`.
3. Les envoyer au processus MPI responsable du domaine d'arrivée (via des communications non-bloquantes ou collectives).

## 5. Synchronisation et Collecte des données

Afin de pouvoir faire le rendu graphique (généralement géré par le processus Root, rank 0), plusieurs mécanismes collectifs sont implémentés :

* **`syncGlobalCount`** : Utilise `MPI_Allreduce` avec `MPI_SUM` pour que tous les processus connaissent le nombre exact total de poissons dans la simulation.
* **`gatherAllBoids`** : Utilise `MPI_Gather` puis `MPI_Gatherv` pour rapatrier la totalité des Boids sur le processus Rank 0, afin de permettre l'affichage SFML de la frame en cours.
* **`scatterAllBoids`** : Permet au processus racine de forcer la position des boids sur l'ensemble du cluster (utile lors d'un reset depuis l'interface ou lors de l'initialisation initiale).

## 6. Bonnes pratiques et Limites actuelles

* **Sérialisation basique** : Actuellement, le transfert des objets `Boid` se fait via le type `MPI_BYTE`. Cela fonctionne car la classe Boid est assimilable à du *Plain Old Data* (POD). S'il devient nécessaire d'ajouter des structures de données complexes (std::vector dynamiques, pointeurs) à l'intérieur d'un Boid, il faudra créer un type dérivé via `MPI_Type_create_struct`.
* **Overhead de communication** : Le `gatherAllBoids` est appelé à chaque frame pour le rendu 2D. Sur un grand nombre de nœuds, ce goulot d'étranglement limitera la scalabilité.