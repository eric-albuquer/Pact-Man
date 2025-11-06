#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "common.h"
#include "chunk.h"

#define START_LIFE 100

typedef struct Player {
    int x;
    int y;

    int lastX;
    int lastY;

    int chunkX;
    int chunkY;

    Direction dir;

    int life;
    int totalFragment;
    int biomeFragment;
    bool fragmentByCoins;
    unsigned short biomeCoins;
    unsigned short totalCoins;
    unsigned biome : 2;
    unsigned slowness : 1;

    bool (*updateChunk)(struct Player*);
    void (*updateDirection)(struct Player*);
    void (*free)(struct Player*);
} Player;

Player* new_Player(int x, int y);

#endif