#include "enemy.h"
#include <stdlib.h>

static void _free(Enemy* this){
    free(this);
}

Enemy* new_Enemy(float x, float y){
    Enemy* this = malloc(sizeof(Enemy));
    this->x = x;
    this->y = y;

    this->free = _free;
    return this;
}