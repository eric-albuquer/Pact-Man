#include "enemy.h"
#include <stdlib.h>

static bool updateChunk(Enemy* this, int chunkSize) {
    int newChunkX = this->x / chunkSize;
    int newChunkY = this->y / chunkSize;
    if (newChunkX != this->chunkX || newChunkY != this->chunkY) {
        this->chunkX = this->x / chunkSize;
        this->chunkY = this->y / chunkSize;
        return true;
    }
    return false;
}

static void updateDirection(Enemy* this){
    int dx = this->x - this->lastX;
    int dy = this->y - this->lastY;
    if (dx == 1) this->dir = RIGHT;
    else if (dx == -1) this->dir = LEFT;
    else if (dy == 1) this->dir = DOWN;
    else if (dy == -1) this->dir = UP;
}

static void _free(Enemy* this){
    free(this);
}

Enemy* new_Enemy(int x, int y, int biomeType){
    Enemy* this = malloc(sizeof(Enemy));
    this->x = x;
    this->y = y;

    this->lastX = x;
    this->lastY = y;

    this->spawnX = x;
    this->spawnY = y;

    this->dir = RIGHT;
    this->changedChunk = false;

    this->biomeType = biomeType;

    this->updateChunk = updateChunk;
    this->updateDirection = updateDirection;
    this->free = _free;
    return this;
}