#ifndef ENEMY_H
#define ENEMY_H

#include "utils.h"

typedef struct Enemy {
    Int64 x;
    Int64 y;

    void (*free)(struct Enemy*);
} Enemy;

Enemy* new_Enemy(Int64 x, Int64 y);

#endif