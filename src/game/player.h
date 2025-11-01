#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "common.h"
#include "chunk.h"

typedef struct Player {
    int x;
    int y;

    int lastX;
    int lastY;

    int chunkX;
    int chunkY;

    Direction dir;

    bool (*updateChunk)(struct Player*, int);
    void (*updateDirection)(struct Player*);
    void (*free)(struct Player*);
} Player;

Player* new_Player(int x, int y);

#endif