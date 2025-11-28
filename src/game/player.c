#include "player.h"
#include <stdlib.h>

//===============================================================
//  ATUALIZAR CHUNK DO PLAYER
//===============================================================

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

//===============================================================
//  ATUALIZAR DIREÇÃO DO PLAYER
//===============================================================

static void updateDirection(Player* this){
    int dx = this->x - this->lastX;
    int dy = this->y - this->lastY;
    if (dx == 1) this->dir = RIGHT;
    else if (dx == -1) this->dir = LEFT;
    else if (dy == 1) this->dir = DOWN;
    else if (dy == -1) this->dir = UP;
}

//===============================================================
//  REINICIAR PLAYER
//===============================================================

static void restart(Player* this){
    this->life = START_LIFE;
    this->speed = 1000;
    this->effects = (Effects){0};
    this->x = this->lastX = this->spawnX;
    this->y = this->lastY = this->spawnY;
    this->damaged = false;
    this->hitEnemy = false;
    this->biomeCoins = this->totalCoins = this->biomeFragment = this->totalFragment = this->totalTime = 0;
    this->alive = 1;
    this->batery = 1.0f;
}

//===============================================================
//  LIBERAR MEMÓRIA
//===============================================================

static void _free(Player* this) { free(this); }

//===============================================================
//  CONSTRUTOR
//===============================================================

Player* new_Player(int x, int y) {
    Player* this = malloc(sizeof(Player));
    this->x = x;
    this->y = y;

    this->lastX = x;
    this->lastY = y;

    this->spawnX = x;
    this->spawnY = y;

    this->life = START_LIFE;
    this->speed = 1000;

    this->biomeCoins = 0;
    this->totalCoins = 0;
    this->totalFragment = 0;
    this->biomeFragment = 0;
    this->getFragment = 0;
    this->biomeChange = 0;
    this->alive = 1;
    this->fragmentByCoins = false;

    this->totalTime = 0;
    this->cellType = CELL_EMPTY;

    this->effects = (Effects){0};
    this->biome = 0;
    this->damaged = false;
    this->hitEnemy = false;
    this->batery = 1.0f;

    this->dir = RIGHT;

    this->updateChunk = updateChunk;
    this->updateDirection = updateDirection;
    this->restart = restart;
    this->free = _free;
    return this;
}