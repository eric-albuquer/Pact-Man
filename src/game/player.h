#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

typedef struct Player {
    Int64 x;
    Int64 y;

    Int64 lastX;
    Int64 lastY;

    void (*free)(struct Player*);
} Player;

Player* new_Player(Int64 x, Int64 y);

#endif