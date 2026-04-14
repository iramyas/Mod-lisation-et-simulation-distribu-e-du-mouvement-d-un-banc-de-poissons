//gestionnaire principale MPI 
//centralise toute la logique mpi  

#include "MPIManager.h"
#include "boid.h"
#include <algorithm>
#include <cmath>

namespace simulation {

// Implémentation de Domain::contains
bool Domain::contains(const Boid& boid) const {
    return boid.position.x >= minX && boid.position.x < maxX &&
           boid.position.y >= minY && boid.position.y < maxY;
}

// Implémentation de Domain::inHalo
bool Domain::inHalo(const Boid& boid, float haloWidth) const {
    float haloMinX = minX - haloWidth;
    float haloMaxX = maxX + haloWidth;
    float haloMinY = minY - haloWidth;
    float haloMaxY = maxY + haloWidth;
    
    return boid.position.x >= haloMinX && boid.position.x < haloMaxX &&
           boid.position.y >= haloMinY && boid.position.y < haloMaxY;
}

MPIManager::MPIManager(float simWidth, float simHeight)
    : simWidth(simWidth), simHeight(simHeight) {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Décomposition cartésienne 2D
    int dims[2] = {0, 0};
    MPI_Dims_create(size, 2, dims);
    procGridX = dims[0];
    procGridY = dims[1];
    
    // Position de ce processus dans la grille
    myProcX = rank % procGridX;
    myProcY = rank / procGridX;
    
    // Calculer le domaine local
    float cellWidth = simWidth / procGridX;
    float cellHeight = simHeight / procGridY;
    
    localDomain.minX = myProcX * cellWidth;
    localDomain.maxX = (myProcX + 1) * cellWidth;
    localDomain.minY = myProcY * cellHeight;
    localDomain.maxY = (myProcY + 1) * cellHeight;
    localDomain.procX = myProcX;
    localDomain.procY = myProcY;
}

void MPIManager::haloExchange(const std::vector<Boid>& localBoids,
                              float neighborRadius,
                              std::vector<Boid>& haloBoids) {
    haloBoids.clear();
    
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;
            
            int neighborX = myProcX + dx;
            int neighborY = myProcY + dy;
            
            if (neighborX < 0) neighborX += procGridX;
            if (neighborX >= procGridX) neighborX -= procGridX;
            if (neighborY < 0) neighborY += procGridY;
            if (neighborY >= procGridY) neighborY -= procGridY;
            
            int neighborRank = neighborY * procGridX + neighborX;
            
            std::vector<Boid> sendBoids;
            for (const auto& boid : localBoids) {
                if (localDomain.inHalo(boid, neighborRadius)) {
                    sendBoids.push_back(boid);
                }
            }
            
            int sendCount = sendBoids.size();
            int recvCount = 0;
            
            // 1. Échange des tailles
            MPI_Sendrecv(&sendCount, 1, MPI_INT, neighborRank, 0,
                        &recvCount, 1, MPI_INT, neighborRank, 0,
                        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // 2. Échange des DONNÉES réelles (Correction ici)
            if (sendCount > 0 || recvCount > 0) {
                std::vector<Boid> recvBoids(recvCount);
                
                MPI_Sendrecv(sendBoids.data(), sendCount * sizeof(Boid), MPI_BYTE, neighborRank, 1,
                             recvBoids.data(), recvCount * sizeof(Boid), MPI_BYTE, neighborRank, 1,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                haloBoids.insert(haloBoids.end(), recvBoids.begin(), recvBoids.end());
            }
        }
    }
}

void MPIManager::migrateBoidsAllToAll(const std::vector<Boid>& localBoids,
                                      std::vector<Boid>& remaining,
                                      std::vector<Boid>& incoming) {
    remaining.clear();
    incoming.clear();
    
    std::vector<std::vector<Boid>> sendBuf(size);
    for (int r = 0; r < size; ++r) sendBuf[r].clear();
    
    for (const auto& boid : localBoids) {
        int tx = static_cast<int>(std::floor(boid.position.x / (simWidth / procGridX)));
        int ty = static_cast<int>(std::floor(boid.position.y / (simHeight / procGridY)));
        if(tx < 0) tx = 0;
        if(ty < 0) ty = 0;
        if(tx >= procGridX) tx = procGridX - 1;
        if(ty >= procGridY) ty = procGridY - 1;
        int targetRank = ty * procGridX + tx;
        if (localDomain.contains(boid)) {
            remaining.push_back(boid);
        } else {
            sendBuf[targetRank].push_back(boid);
        }
    }
    // On compte le nombre d'échanges
    std::vector<int> sendCounts(size, 0), recvCounts(size,0);
    for (int r = 0; r < size; ++r) sendCounts[r] = static_cast<int>(sendBuf[r].size());
    MPI_Alltoall(sendCounts.data(), 1, MPI_INT, recvCounts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    // On construit un buffer d'envoi continue et on calcule le déplacement
    std::vector<int> sdispls(size, 0), rdispls(size, 0);
    int sTotal = 0, rTotal = 0;
    for (int i = 0; i < size; ++i) { 
        sdispls[i] = sTotal * static_cast<int>(sizeof(Boid));
        rdispls[i] = rTotal * static_cast<int>(sizeof(Boid));
        sTotal += sendCounts[i];
        rTotal += recvCounts[i];
    }

    std::vector<Boid> sdata;
    sdata.reserve(sTotal);
    for (int i = 0; i < size; ++i) {
        for (const auto& b : sendBuf[i]) sdata.push_back(b);
    }
    std::vector<Boid> rdata;
    if(rTotal > 0) rdata.resize(rTotal);

    // On convertie les compteurs en bytes pour utiliser MPI_BYTE
    std::vector<int> sendCountsBytes(size, 0), recvCountsBytes(size, 0);
    for(int i = 0; i < size; ++i) {
        sendCountsBytes[i] = sendCounts[i] * static_cast<int>(sizeof(Boid));
        recvCountsBytes[i] = recvCounts[i] * static_cast<int>(sizeof(Boid));
    }

    MPI_Alltoallv(sdata.data(), sendCountsBytes.data(), sdispls.data(), MPI_BYTE,
                    rdata.data(), recvCountsBytes.data(), rdispls.data(), MPI_BYTE,
                    MPI_COMM_WORLD);
    
    // Déplace les données reçues dans le tableau incoming
    if (rTotal > 0) incoming = std::move(rdata);
}

int MPIManager::syncGlobalCount(int localCount) {
    int globalCount = 0;
    MPI_Allreduce(&localCount, &globalCount, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    return globalCount;
}

void MPIManager::gatherBoids(const std::vector<Boid>& localBoids, std::vector<Boid>& globalBoids) {
    int localSize = localBoids.size();
    std::vector<int> recvCounts(size);
    
    // 1. Rassembler les tailles
    MPI_Gather(&localSize, 1, MPI_INT, recvCounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    std::vector<int> displacements(size, 0);
    int totalSize = 0;
    
    // 2. Calculer les déplacements pour Gatherv
    if (rank == 0) {
        for (int i = 0; i < size; ++i) {
            displacements[i] = totalSize * sizeof(Boid);
            recvCounts[i] *= sizeof(Boid); // Conversion en octets
            totalSize += recvCounts[i] / sizeof(Boid);
        }
        globalBoids.resize(totalSize);
    }
    
    // 3. Récupérer les données
    MPI_Gatherv(localBoids.data(), localSize * sizeof(Boid), MPI_BYTE,
                globalBoids.data(), recvCounts.data(), displacements.data(), 
                MPI_BYTE, 0, MPI_COMM_WORLD);
}

void MPIManager::scatterBoids(const std::vector<Boid>& globalBoids,
                              std::vector<Boid>& localBoids) {
    int localSize = 0;
    
    if (rank == 0) {
        localSize = globalBoids.size() / size;
    }
    
    MPI_Bcast(&localSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    localBoids.resize(localSize);
    if (rank == 0) {
        std::copy(globalBoids.begin(), globalBoids.begin() + localSize,
                 localBoids.begin());
    }
    
    // Broadcast à tous les processus
    MPI_Bcast(localBoids.data(), localSize * sizeof(Boid), MPI_BYTE, 0, MPI_COMM_WORLD);
}

void MPIManager::printInfo() const {
    std::cout << "Rank " << rank << ": domain (" << localDomain.minX << ", "
              << localDomain.minY << ") to (" << localDomain.maxX << ", "
              << localDomain.maxY << ")" << std::endl;
}

} // namespace simulation
