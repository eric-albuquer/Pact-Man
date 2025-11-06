#include "pathfinding.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "linkedlist.h"

struct QNode {
    int x, y, d;
};
typedef struct QNode QNode;

static inline bool farAway(Player* p, Enemy* e) {
    float distToSpawn = hypotf(p->x - e->spawnX, p->y - e->spawnY);

    return distToSpawn > MAX_PERSUIT_RADIUS;
}

void mapDistancePlayer(Map* map) {
    if (!map) return;

    ChunkManager* cm = map->manager;
    Chunk** adjacents = cm->adjacents;

    for (int i = 0; i < 9; i++) {
        int idx = CLOSER_IDX[i];
        Chunk* chunk = adjacents[idx];
        if (chunk) chunk->resetDistance(chunk);
    }

    LinkedList* BFSQueue = new_LinkedList();
    QNode* start = malloc(sizeof(*start));

    start->x = map->player->x;
    start->y = map->player->y;
    start->d = 0;

    BFSQueue->addLast(BFSQueue, start);
    Cell* cell = cm->getUpdatedCell(cm, start->x, start->y);
    cell->distance = 0;

    static const int DX[4] = {1, -1, 0, 0};
    static const int DY[4] = {0, 0, 1, -1};

    while (BFSQueue->length > 0) {
        QNode* curr = BFSQueue->removeFirst(BFSQueue);
        for (int i = 0; i < 4; i++) {
            int neighborX = curr->x + DX[i];
            int neighborY = curr->y + DY[i];

            Cell* next = cm->getUpdatedCell(cm, neighborX, neighborY);
            if (!next || next->type == CELL_WALL || next->distance != -1) continue;

            next->distance = curr->d + 1;
            QNode* neighborNew = malloc(sizeof(*neighborNew));
            neighborNew->x = neighborX;
            neighborNew->y = neighborY;
            neighborNew->d = curr->d + 1;
            BFSQueue->addLast(BFSQueue, neighborNew);
        }
        free(curr);
    }
    BFSQueue->free(BFSQueue);
}

bool enemyStepTowardsPlayer(Map* map, Enemy* e) {
    if (!map || !e) return false;
    ChunkManager* cm = map->manager;

    int enemyPosX = e->x, enemyPosY = e->y;

    Cell* cell = cm->getUpdatedCell(cm, enemyPosX, enemyPosY);
    if (!cell || cell->distance <= 0) return false;

    static const int DX[4] = {0, 1, 0, -1};
    static const int DY[4] = {-1, 0, 1, 0};

    static QNode moves[4];
    int length = 0;

    for (int i = 0; i < 4; i++) {
        int neighborX = enemyPosX + DX[i];
        int neighborY = enemyPosY + DY[i];

        Cell* next = cm->getUpdatedCell(cm, neighborX, neighborY);
        if (!next || next->type == CELL_WALL || next->biome != e->biome) continue;

        moves[length++] = (QNode){neighborX, neighborY, next->distance};
    }

    // ordenando direções
    for (int i = 0; i < length; i++) {
        for (int j = i + 1; j < length; j++) {
            if (moves[i].d > moves[j].d) {
                QNode temp = moves[i];
                moves[i] = moves[j];
                moves[j] = temp;
            }
        }
    }

    if (length == 0) return false;

    // Indo para a pior direção
    if (farAway(map->player, e)) {
        e->x = moves[length - 1].x;
        e->y = moves[length - 1].y;
        return true;
    }

    int nx = moves[0].x;
    int ny = moves[0].y;

    // probabilidade de seguir melhor caminho ou um aleatório
    float random = rand() / (float)RAND_MAX;
    if (random > BEST_PATH_PROBABILITY) {
        int idx = rand() % length;
        QNode choosed = moves[idx];
        nx = choosed.x;
        ny = choosed.y;
    }

    e->x = nx;
    e->y = ny;
    return true;
}
