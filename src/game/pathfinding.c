#include <stdlib.h>

#include "pathfinding.h"
#include "linkedlist.h"

struct QNode { int x, y, d; };
typedef struct QNode QNode;

static inline int inBounds(Map *map, int x, int y) {
    return x >= 0 && y >= 0 && x < map->cols && y < map->rows;
}

static inline int passable(Map* map, int x, int y) {
    return !map->matrix[y][x].isWall;
}

void mapDistancePlayer(Map *map, int max_distance) {
    if (!map || !map->player) return;

    int **dist = map->dist;
    const int ROWS = map->rows, COLUMNS = map->cols;

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0 ; x < COLUMNS; x++) {
            dist[y][x] = -1;
        }
    }

    LinkedList *BFSQueue = new_LinkedList();
    QNode *start = malloc(sizeof(*start));

    start->x = map->player->x;
    start->y = map->player->y;
    start->d = 0;

    if (!inBounds(map, start->x, start->y) || !passable(map, start->x, start->y)) {
        free(start);
        BFSQueue->free(BFSQueue);
        return;
    }

    BFSQueue->addLast(BFSQueue, start);
    dist[start->y][start->x] = 0;

    static const int DX[4] = { 1, -1, 0, 0 };
    static const int DY[4] = { 0, 0, 1, -1 };

    while (BFSQueue->length) {
        QNode *curr = BFSQueue->removeFirst(BFSQueue);
        if (max_distance > 0 && curr->d >= max_distance) { free(curr); continue; }

        for (int i = 0; i < 4; i++) {
            int neighborX = curr->x + DX[i];
            int neighborY = curr->y + DY[i];

            if (!inBounds(map, neighborX, neighborY)) continue;
            if (!passable(map, neighborX, neighborY)) continue;
            if (dist[neighborY][neighborX] != -1) continue;

            dist[neighborY][neighborX] = curr->d + 1;
            QNode *neighborNew = malloc(sizeof(*neighborNew));
            neighborNew->x = neighborX; neighborNew->y = neighborY; neighborNew->d = curr->d + 1;
            BFSQueue->addLast(BFSQueue, neighborNew);
        }
        free(curr);
    }
    BFSQueue->free(BFSQueue);
}

int enemyStepTowardsPlayer(Map *map, Enemy *e) {
    if (!map || !e) return 0;

    int **dist = map->dist;
    int enemyPosX = e->x, enemyPosY;
    
    if (!inBounds(map, enemyPosX, enemyPosY)) return 0;
    if (dist[enemyPosY][enemyPosX] <= 0) return 0;

    static const int DX[4] = { 0, 1, 0, -1};
    static const int DY[4] = { -1, 0, 1, 0};

    int bestDistX = 0, bestDistY = 0, bestVal = dist[enemyPosY][enemyPosX];

    for (int i = 0; i < 4; i++) {
        int neighborX = enemyPosX + DX[i];
        int neighborY = enemyPosY + DY[i];

        if (!inBounds(map, neighborX, neighborY)) continue;
        if (!passable(map, neighborX, neighborY)) continue;

        int value = dist[neighborY][neighborX];

        if (value >= 0 && value < bestVal) {
            bestVal = value;
            bestDistX = DX[i];
            bestDistY = DY[i];
        }
    }

    if (bestVal < dist[enemyPosY][enemyPosX]) {
        e->x = enemyPosX + bestDistX;
        e->y = enemyPosY + bestDistY;
        return 1;
    }
    return 0;
}
