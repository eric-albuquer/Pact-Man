#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>
#include "common.h"

typedef struct Enemy {
    int x;
    int y;

    int lastX;
    int lastY;

    int spawnX;
    int spawnY;

    int chunkX;
    int chunkY;

    Direction dir;

    int biomeType;

    bool (*updateChunk)(struct Enemy*, int);
    void (*free)(struct Enemy*);
} Enemy;

Enemy* new_Enemy(int x, int y, int biomeType);

#endif