#include "player.h"
#include <stdlib.h>

static void _free(Player* this){
    free(this);
}

Player* new_Player(float x, float y){
    Player* this = malloc(sizeof(Player));
    this->x = x;
    this->y = y;

    this->free = _free;
    return this;
}