#include "MPIManager.h"
#include "boid.h"
#include <cmath>

namespace simulation {


/// @brief Vérifie si un boid est contenu dans le domaine local
///
/// Teste en utilisant des comparaisons d'intervalle fermé-ouvert [minX, maxX).
/// @param boid Boid à tester
/// @return true si le boid appartient au domaine, false sinon
/// @note Utilise des intervalles [min, max) pour éviter les doublons sur les bords
bool Domain::contains(const Boid& boid) const {
    return boid.position.x >= minX && boid.position.x < maxX &&
           boid.position.y >= minY && boid.position.y < maxY;
}

/// @brief Vérifie si un boid est dans la zone halo (bord) du domaine
///
/// Étend le domaine de `haloWidth` dans toutes les directions pour
/// détecter les boids qui doivent être échangés avec les voisins.
/// @param boid Boid à tester
/// @param haloWidth Épaisseur de la zone halo (rayon de voisinage)
/// @return true si le boid est dans la zone halo, false sinon
/// @note La zone halo s'étend de haloWidth dans toutes les 8 directions
bool Domain::inHalo(const Boid& boid, float haloWidth) const {
    float haloMinX = minX - haloWidth;
    float haloMaxX = maxX + haloWidth;
    float haloMinY = minY - haloWidth;
    float haloMaxY = maxY + haloWidth;

    return boid.position.x >= haloMinX && boid.position.x < haloMaxX &&
           boid.position.y >= haloMinY && boid.position.y < haloMaxY;
}

/// @brief Constructeur : initialise le gestionnaire MPI avec décomposition 2D
///
/// - Récupère le rang et la taille du communicateur MPI
/// - Crée une grille Cartésienne 2D via MPI_Dims_create()
/// - Calcule le domaine spatial local en fonction de la position dans la grille
///
/// @param simWidth Largeur totale de la simulation
/// @param simHeight Hauteur totale de la simulation
///
/// @note La grille est automatiquement équilibrée par MPI_Dims_create()
/// @note Chaque processus calcule ses limites de domaine en fonction de sa position (myProcX, myProcY)
MPIManager::MPIManager(float simWidth, float simHeight)
    : simWidth(simWidth), simHeight(simHeight) {

    // Récupérer rang et taille
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Décomposition cartésienne 2D
    int dims[2] = {0, 0};
    MPI_Dims_create(size, 2, dims);
    procGridX = dims[0];
    procGridY = dims[1];

    // Calculer position du processus dans la grille
    myProcX = rank % procGridX;
    myProcY = rank / procGridX;

    // Calculer le domaine spatial local
    float cellWidth = simWidth / procGridX;
    float cellHeight = simHeight / procGridY;

    localDomain.minX = myProcX * cellWidth;
    localDomain.maxX = (myProcX + 1) * cellWidth;
    localDomain.minY = myProcY * cellHeight;
    localDomain.maxY = (myProcY + 1) * cellHeight;
    localDomain.procX = myProcX;
    localDomain.procY = myProcY;
}

/// @brief Échange les boids des zones frontières avec les 8 voisins (toroïdal)
///
/// Envoie les boids proches de la frontière locale à chaque voisin.
/// Reçoit en retour les boids proches de la frontière des voisins.
///
/// Étapes:
/// 1. Pour chaque voisin (8 directions avec wrap toroïdal)
/// 2. Sélectionner les boids locaux qui sont proches du bord
/// 3. Échange bidirectionnel MPI_Sendrecv (taille, puis données)
/// 4. Accumuler tous les boids reçus dans haloBoids
///
/// @param localBoids Boids actuellement au sein du domaine local
/// @param neighborRadius Rayon de détection pour les bois frontière
/// @param haloBoids [OUT] Boids provenant des domaines voisins
///
/// @note Utilise MPI_Sendrecv pour éviter les deadlocks
/// @note Utilise MPI_BYTE pour flexibilité (sizeof(Boid) peut varier)
/// @note Mesh toroïdal : wrap-around aux bords de la simulation
void MPIManager::haloExchange(const std::vector<Boid>& localBoids,
                              float neighborRadius,
                              std::vector<Boid>& haloBoids) {
    haloBoids.clear();

    // Itérer sur les 8 directions de voisins (3x3 grid, sans centre)
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;  // Ignorer le centre (soi-même)

            // Calculer les coordonnées du voisin
            int neighborX = myProcX + dx;
            int neighborY = myProcY + dy;

            // Appliquer wrap-around toroïdal
            if (neighborX < 0) neighborX += procGridX;
            if (neighborX >= procGridX) neighborX -= procGridX;
            if (neighborY < 0) neighborY += procGridY;
            if (neighborY >= procGridY) neighborY -= procGridY;

            // Convertir coordonnées (neighborX, neighborY) en rang MPI
            int neighborRank = neighborY * procGridX + neighborX;

            // Sélectionner les boids à envoyer :
            // Uniquement ceux qui sont dans le domaine local ET proches
            // de la frontière dans la direction (dx, dy) du voisin.
            std::vector<Boid> sendBoids;
            for (const auto& boid : localBoids) {
                bool nearBorder = true;
                // Si dx == +1 : le voisin est à droite → envoyer les boids proches du bord droit
                if (dx == +1 && boid.position.x < localDomain.maxX - neighborRadius) nearBorder = false;
                // Si dx == -1 : le voisin est à gauche → envoyer les boids proches du bord gauche
                if (dx == -1 && boid.position.x > localDomain.minX + neighborRadius) nearBorder = false;
                // Si dy == +1 : le voisin est en bas → envoyer les boids proches du bord bas
                if (dy == +1 && boid.position.y < localDomain.maxY - neighborRadius) nearBorder = false;
                // Si dy == -1 : le voisin est en haut → envoyer les boids proches du bord haut
                if (dy == -1 && boid.position.y > localDomain.minY + neighborRadius) nearBorder = false;
                if (nearBorder) {
                    sendBoids.push_back(boid);
                }
            }

            int sendCount = sendBoids.size();
            int recvCount = 0;

            // 1. Échange des tailles (MPI_Sendrecv bidirectionnel)
            MPI_Sendrecv(&sendCount, 1, MPI_INT, neighborRank, 0,
                        &recvCount, 1, MPI_INT, neighborRank, 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // 2. Échange des données si count > 0
            if (sendCount > 0 || recvCount > 0) {
                std::vector<Boid> recvBoids(recvCount);

                MPI_Sendrecv(sendBoids.data(), sendCount * sizeof(Boid), MPI_BYTE, neighborRank, 1,
                             recvBoids.data(), recvCount * sizeof(Boid), MPI_BYTE, neighborRank, 1,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Ajouter les boids reçus au buffer halo
                haloBoids.insert(haloBoids.end(), recvBoids.begin(), recvBoids.end());
            }
        }
    }
}

/// @brief Effectue la migration des boids entre domaines via MPI_Alltoallv
///
/// Classifie les boids en fonction de leur domaine cible et les envoie
/// aux processus appropriés. Les boids restant dans le domaine local
/// sont séparés des boids à migrer.
///
/// Étapes:
/// 1. Classer chaque boid selon son domaine cible
/// 2. Utiliser MPI_Alltoall pour échanger les compteurs
/// 3. Utiliser MPI_Alltoallv pour envoyer/recevoir les données
///
/// @param localBoids Tous les boids du domaine courant
/// @param remaining [OUT] Boids qui restent (contenu dans le domaine local)
/// @param incoming [OUT] Boids qui arrivent d'autres domaines
///
/// @note Utilise MPI_Alltoallv pour communication efficace all-to-all
/// @note Convertit les compteurs en bytes pour MPI_BYTE
/// @note Boids restants et boids entrants sont disjoints et sans doublons
void MPIManager::migrateBoidsAllToAll(const std::vector<Boid>& localBoids,
                                      std::vector<Boid>& remaining,
                                      std::vector<Boid>& incoming) {
    remaining.clear();
    incoming.clear();

    // 1. Construire les buffers d'envoi par destination
    std::vector<std::vector<Boid>> sendBuf(size);
    for (int r = 0; r < size; ++r) sendBuf[r].clear();

    // Classer les boids selon leur domaine cible
    for (const auto& boid : localBoids) {
        // Calculer le domaine cible en fonction de la position du boid
        int tx = static_cast<int>(std::floor(boid.position.x / (simWidth / procGridX)));
        int ty = static_cast<int>(std::floor(boid.position.y / (simHeight / procGridY)));

        // Clamp aux limites de la grille
        if(tx < 0) tx = 0;
        if(ty < 0) ty = 0;
        if(tx >= procGridX) tx = procGridX - 1;
        if(ty >= procGridY) ty = procGridY - 1;

        // Convertir coordonnées en rang
        int targetRank = ty * procGridX + tx;

        // Classer : si le boid reste local, le garder; sinon l'envoyer
        if (localDomain.contains(boid)) {
            remaining.push_back(boid);
        } else {
            sendBuf[targetRank].push_back(boid);
        }
    }

    // 2. Échange des compteurs d'envoi/réception (MPI_Alltoall)
    std::vector<int> sendCounts(size, 0), recvCounts(size, 0);
    for (int r = 0; r < size; ++r) {
        sendCounts[r] = static_cast<int>(sendBuf[r].size());
    }
    MPI_Alltoall(sendCounts.data(), 1, MPI_INT, recvCounts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    // 3. Construire les déplacements et buffer continu d'envoi
    std::vector<int> sdispls(size, 0), rdispls(size, 0);
    int sTotal = 0, rTotal = 0;
    for (int i = 0; i < size; ++i) {
        sdispls[i] = sTotal * static_cast<int>(sizeof(Boid));
        rdispls[i] = rTotal * static_cast<int>(sizeof(Boid));
        sTotal += sendCounts[i];
        rTotal += recvCounts[i];
    }

    // Construire le buffer d'envoi continu
    std::vector<Boid> sdata;
    sdata.reserve(sTotal);
    for (int i = 0; i < size; ++i) {
        for (const auto& b : sendBuf[i]) sdata.push_back(b);
    }

    // Allouer le buffer de réception
    std::vector<Boid> rdata;
    if(rTotal > 0) rdata.resize(rTotal);

    // 4. Convertir les compteurs en bytes (pour MPI_BYTE)
    std::vector<int> sendCountsBytes(size, 0), recvCountsBytes(size, 0);
    for(int i = 0; i < size; ++i) {
        sendCountsBytes[i] = sendCounts[i] * static_cast<int>(sizeof(Boid));
        recvCountsBytes[i] = recvCounts[i] * static_cast<int>(sizeof(Boid));
    }

    // 5. Effectuer la migration all-to-all
    MPI_Alltoallv(sdata.data(), sendCountsBytes.data(), sdispls.data(), MPI_BYTE,
                    rdata.data(), recvCountsBytes.data(), rdispls.data(), MPI_BYTE,
                    MPI_COMM_WORLD);

    // Déplacer les données reçues dans incoming
    if (rTotal > 0) incoming = std::move(rdata);
}

/// @brief Synchronise le nombre global de boids via MPI_Allreduce (SUM)
///
/// Somme le nombre de boids local sur tous les processus.
/// Utile pour vérifier que le nombre total est conservé (validation).
///
/// @param localCount Nombre de boids dans le domaine local
/// @return Nombre total de boids sumé sur tous les processus
///
/// @note Utilise MPI_Allreduce avec MPI_SUM
/// @note Chaque process reçoit le résultat global
int MPIManager::syncGlobalCount(int localCount) {
    int globalCount = 0;
    MPI_Allreduce(&localCount, &globalCount, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    return globalCount;
}

/// @brief Rassemble tous les boids sur le rank 0 (MPI_Gatherv)
///
/// - Rank 0 reçoit tous les boids de tous les processus
/// - Les autres ranks n'accélèrent que leurs données
///
/// @param localBoids Boids du processus courant
/// @param globalBoids [OUT] (rank 0 seulement) Tous les boids fusionnés
///
/// @note Seul le rank 0 aura une globalBoids valide et complète
/// @note Les autres ranks auront une globalBoids vide
void MPIManager::gatherBoids(const std::vector<Boid>& localBoids, std::vector<Boid>& globalBoids) {
    int localSize = localBoids.size();
    std::vector<int> recvCounts(size);

    // 1. Rassembler les tailles via MPI_Gather
    MPI_Gather(&localSize, 1, MPI_INT, recvCounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> displacements(size, 0);
    int totalSize = 0;

    // 2. Calculer les déplacements (rank 0 seulement)
    if (rank == 0) {
        // D'abord accumuler totalSize en nombre de Boids et calculer les déplacements
        for (int i = 0; i < size; ++i) {
            displacements[i] = totalSize * static_cast<int>(sizeof(Boid));
            totalSize += recvCounts[i];
        }
        globalBoids.resize(totalSize);
        // Ensuite convertir recvCounts en bytes pour MPI_Gatherv
        for (int i = 0; i < size; ++i) {
            recvCounts[i] *= static_cast<int>(sizeof(Boid));
        }
    }

    // 3. Rassembler les données via MPI_Gatherv
    MPI_Gatherv(localBoids.data(), localSize * sizeof(Boid), MPI_BYTE,
                globalBoids.data(), recvCounts.data(), displacements.data(),
                MPI_BYTE, 0, MPI_COMM_WORLD);
}

/// @brief Distribute les boids depuis le rank 0 à tous les processus (MPI_Scatterv)
///
/// - Rank 0 envoie les boids à tous les autres processus
/// - Répartit globalboids en parts égales
///
/// @param globalBoids Boids source (rank 0 seulement)
/// @param localBoids [OUT] Boids reçus par ce processus
///
/// @note Chaque processus reçoit globalBoids.size() / size boids
void MPIManager::scatterBoids(const std::vector<Boid>& globalBoids,
                              std::vector<Boid>& localBoids) {
    // 1. Rank 0 calcule la taille pour chaque processus
    std::vector<int> sendCounts(size, 0);
    std::vector<int> sendDispls(size, 0);

    if (rank == 0) {
        int base = static_cast<int>(globalBoids.size()) / size;
        int remainder = static_cast<int>(globalBoids.size()) % size;
        int offset = 0;
        for (int i = 0; i < size; ++i) {
            // Distribuer le reste sur les premiers processus
            sendCounts[i] = base + (i < remainder ? 1 : 0);
            sendDispls[i] = offset * static_cast<int>(sizeof(Boid));
            offset += sendCounts[i];
        }
    }

    // 2. Broadcast la taille que ce processus va recevoir
    int localSize = 0;
    MPI_Scatter(sendCounts.data(), 1, MPI_INT,
                &localSize, 1, MPI_INT,
                0, MPI_COMM_WORLD);

    // 3. Convertir sendCounts en bytes pour MPI_Scatterv
    std::vector<int> sendCountsBytes(size, 0);
    for (int i = 0; i < size; ++i) {
        sendCountsBytes[i] = sendCounts[i] * static_cast<int>(sizeof(Boid));
    }

    // 4. Scatter les boids
    localBoids.resize(localSize);
    MPI_Scatterv(globalBoids.data(), sendCountsBytes.data(), sendDispls.data(), MPI_BYTE,
                 localBoids.data(), localSize * static_cast<int>(sizeof(Boid)), MPI_BYTE,
                 0, MPI_COMM_WORLD);
}

/// @brief Rassemble tous les boids sur TOUS les processus (MPI_Allgatherv)
///
/// Contrairement à gatherBoids (centralisation sur rank 0),
/// chaque processus reçoit UNE COPIE de tous les boids.
/// Utile pour les tests et comparaisons globales.
///
/// Étapes:
/// 1. MPI_Allgather pour collecter les compteurs de tailles
/// 2. Calculer les déplacements
/// 3. MPI_Allgatherv pour envoyer/recevoir les données
///
/// @param localBoids Boids du processus courant
/// @param allBoids [OUT] Tous les boids de la simulation globale (sur tous les ranks)
///
/// @note Chaque processus aura une copie COMPLÈTE de allBoids
/// @note Utilise MPI_Allgatherv pour flexibilité de tailles variables
/// @note Les données sont contiguës et sans ordre particulier
void MPIManager::allGatherBoids(const std::vector<Boid>& localBoids,
                                std::vector<Boid>& allBoids) {
    // 1. Envoyer les tailles locales et recevoir les tailles de tous
    int localSize = static_cast<int>(localBoids.size());
    std::vector<int> allSizes(size);

    MPI_Allgather(&localSize, 1, MPI_INT,
                  allSizes.data(), 1, MPI_INT,
                  MPI_COMM_WORLD);

    // 2. Calculer les déplacements pour Allgatherv
    std::vector<int> displacements(size, 0);
    int totalSize = 0;
    for (int i = 0; i < size; ++i) {
        displacements[i] = totalSize * static_cast<int>(sizeof(Boid));
        allSizes[i] *= static_cast<int>(sizeof(Boid));  // Convertir en bytes
        totalSize += allSizes[i] / static_cast<int>(sizeof(Boid));
    }

    // 3. Allouer le buffer de réception et appeler Allgatherv
    allBoids.resize(totalSize);

    MPI_Allgatherv(localBoids.data(), localSize * static_cast<int>(sizeof(Boid)), MPI_BYTE,
                   allBoids.data(), allSizes.data(), displacements.data(), MPI_BYTE,
                   MPI_COMM_WORLD);
}

/// @brief Affiche les informations du domaine local (rank, limites spatiales)
///
/// Utile pour debug et vérification de la décomposition.
void MPIManager::printInfo() const {
    std::cout << "Rank " << rank << ": domain (" << localDomain.minX << ", "
              << localDomain.minY << ") to (" << localDomain.maxX << ", "
              << localDomain.maxY << ")" << std::endl;
}

} // namespace simulation
