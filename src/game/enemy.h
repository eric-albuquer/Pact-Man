#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>

typedef struct Enemy {
    int x;
    int y;

    int spawnX;
    int spawnY;

    int chunkX;
    int chunkY;

    int biomeType;

    bool (*updateChunk)(struct Enemy*, int);
    void (*free)(struct Enemy*);
} Enemy;

Enemy* new_Enemy(int x, int y, int biomeType);

#endif