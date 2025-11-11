#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>
#include "chunk.h"
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

    unsigned changedChunk : 1;
    unsigned isBoss: 1;

    Direction dir;

    unsigned biome: 2;

    bool (*updateChunk)(struct Enemy*);
    void (*updateDirection)(struct Enemy*);
    void (*free)(struct Enemy*);
} Enemy;

Enemy* new_Enemy(int x, int y, int biome);

#endif