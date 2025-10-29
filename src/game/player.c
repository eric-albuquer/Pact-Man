#include "player.h"
#include <stdlib.h>

static bool updateChunk(Player* this, int chunkSize) {
    int newChunkX = this->x / chunkSize;
    int newChunkY = this->y / chunkSize;
    if (newChunkX != this->chunkX || newChunkY != this->chunkY) {
        this->chunkX = this->x / chunkSize;
        this->chunkY = this->y / chunkSize;
        return true;
    }
    return false;
}

static void updateDirection(Player* this){
    int dx = this->x - this->lastX;
    int dy = this->y - this->lastY;
    if (dx == 1) this->dir = RIGHT;
    else if (dx == -1) this->dir = LEFT;
    else if (dy == 1) this->dir = DOWN;
    else if (dy == -1) this->dir = UP;
}

static void _free(Player* this) { free(this); }

Player* new_Player(int x, int y) {
    Player* this = malloc(sizeof(Player));
    this->x = x;
    this->y = y;

    this->lastX = x;
    this->lastY = y;

    this->dir = RIGHT;

    this->updateChunk = updateChunk;
    this->updateDirection = updateDirection;
    this->free = _free;
    return this;
}