#include "player.h"
#include <stdlib.h>

static void _free(Player* this){
    free(this);
}

Player* new_Player(Int64 x, Int64 y){
    Player* this = malloc(sizeof(Player));
    this->x = x;
    this->y = y;

    this->lastX = x;
    this->lastY = y;

    this->free = _free;
    return this;
}