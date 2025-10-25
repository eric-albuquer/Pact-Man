#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

typedef struct Player {
    float x;
    float y;

    void (*free)(struct Player*);
} Player;

Player* new_Player(float x, float y);

#endif