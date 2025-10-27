#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

typedef struct Player {
    int x;
    int y;

    int chunkX;
    int chunkY;

    bool (*updateChunk)(struct Player*, int);
    void (*free)(struct Player*);
} Player;

Player* new_Player(int x, int y);

#endif