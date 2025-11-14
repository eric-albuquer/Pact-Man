#include "player.h"
#include <stdlib.h>

static bool updateChunk(Player* this) {
    int newChunkX = this->x >> CHUNK_SHIFT;
    int newChunkY = this->y >> CHUNK_SHIFT;
    if (newChunkX != this->chunkX || newChunkY != this->chunkY) {
        this->chunkX = newChunkX;
        this->chunkY = newChunkY;
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

    this->life = START_LIFE;
    this->biomeCoins = 0;
    this->totalCoins = 0;
    this->totalFragment = 0;
    this->biomeFragment = 0;
    this->fragmentByCoins = false;

    this->effects = (Effects){0};
    this->biome = 0;
    this->damaged = false;
    this->hitEnemy = false;
    this->batery = 1.0f;

    this->dir = RIGHT;

    this->updateChunk = updateChunk;
    this->updateDirection = updateDirection;
    this->free = _free;
    return this;
}