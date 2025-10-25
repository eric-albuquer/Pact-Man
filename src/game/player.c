#include "player.h"
#include <stdlib.h>

Player* new_Player(float x, float y){
    Player* this = malloc(sizeof(Player));
    this->x = x;
    this->y = y;
    return this;
}