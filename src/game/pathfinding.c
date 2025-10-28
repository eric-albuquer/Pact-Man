#include "pathfinding.h"

#include <math.h>
#include <stdlib.h>

#include "linkedlist.h"

struct QNode {
    int x, y, d;
};
typedef struct QNode QNode;

static inline int inBounds(Map* map, int x, int y) {
    return x >= 0 && y >= 0 && x < map->cols && y < map->rows;
}

static inline int passable(Map* map, int x, int y) {
    return !map->matrix[y][x].isWall;
}

static inline int sameBiome(Map* map, int x, int y, Enemy* e){
    return map->matrix[y][x].biomeType == e->biomeType;
}

static inline bool farAway(Map* map, Enemy* e){
    float distToSpawn = hypotf(map->player->x - e->spawnX, map->player->y - e->spawnY);

    return distToSpawn > MAX_PERSUIT_RADIUS;
}

static inline int max(int a, int b) { return a > b ? a : b; }

static inline int min(int a, int b) { return a < b ? a : b; }

void mapDistancePlayer(Map* map, int max_distance) {
    if (!map || !map->player) return;

    Cell** matrix = map->matrix;
    const int ROWS = map->rows, COLUMNS = map->cols;

    int minY = max(map->player->y - max_distance - 1, 0);
    int maxY = min(map->player->y + max_distance + 1, ROWS - 1);
    int minX = max(map->player->x - max_distance - 1, 0);
    int maxX = min(map->player->x + max_distance + 1, COLUMNS - 1);
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            matrix[y][x].distance = -1;
        }
    }

    LinkedList* BFSQueue = new_LinkedList();
    QNode* start = malloc(sizeof(*start));

    start->x = map->player->x;
    start->y = map->player->y;
    start->d = 0;

    if (!inBounds(map, start->x, start->y) ||
        !passable(map, start->x, start->y)) {
        free(start);
        BFSQueue->free(BFSQueue);
        return;
    }

    BFSQueue->addLast(BFSQueue, start);
    matrix[start->y][start->x].distance = 0;

    static const int DX[4] = {1, -1, 0, 0};
    static const int DY[4] = {0, 0, 1, -1};

    while (BFSQueue->length) {
        QNode* curr = BFSQueue->removeFirst(BFSQueue);
        if (max_distance > 0 && curr->d >= max_distance) {
            free(curr);
            continue;
        }

        for (int i = 0; i < 4; i++) {
            int neighborX = curr->x + DX[i];
            int neighborY = curr->y + DY[i];

            if (!inBounds(map, neighborX, neighborY)) continue;
            if (!passable(map, neighborX, neighborY)) continue;
            if (matrix[neighborY][neighborX].distance != -1) continue;

            matrix[neighborY][neighborX].distance = curr->d + 1;
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
    if (!map || !e) return 0;

    Cell** matrix = map->matrix;
    int enemyPosX = e->x, enemyPosY = e->y;

    if (!inBounds(map, enemyPosX, enemyPosY)) return 0;
    if (matrix[enemyPosY][enemyPosX].distance <= 0) return 0;

    static const int DX[4] = {0, 1, 0, -1};
    static const int DY[4] = {-1, 0, 1, 0};

    static QNode moves[4];
    int length = 0;

    // int bestDistX = 0, bestDistY = 0, bestVal =
    // matrix[enemyPosY][enemyPosX].distance;

    for (int i = 0; i < 4; i++) {
        int neighborX = enemyPosX + DX[i];
        int neighborY = enemyPosY + DY[i];

        if (!inBounds(map, neighborX, neighborY)) continue;
        if (!passable(map, neighborX, neighborY)) continue;
        if (!sameBiome(map, neighborX, neighborY, e)) continue;
        int dist = matrix[neighborY][neighborX].distance;
        moves[length++] = (QNode){neighborX, neighborY, dist};

        // if (value >= 0 && value < bestVal) {
        //     bestVal = value;
        //     bestDistX = DX[i];
        //     bestDistY = DY[i];
        // }
    }

    for (int i = 0; i < length; i++) {
        for (int j = i + 1; j < length; j++) {
            if (moves[i].d > moves[j].d) {
                QNode temp = moves[i];
                moves[i] = moves[j];
                moves[j] = temp;
            }
        }
    }

    if (farAway(map, e)){
        e->x = moves[length - 1].x;
        e->y = moves[length - 1].y;
        return true;
    }

    int nx = moves[0].x;
    int ny = moves[0].y;

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
