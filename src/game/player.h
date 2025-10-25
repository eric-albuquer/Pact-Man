#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"

typedef struct {
    float x;
    float y;
} Player;

Player* new_Player(float x, float y);

#endif