#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "common.h"
#include "chunk.h"

#define START_LIFE 100

typedef struct Effect {
    unsigned strenght: 8;
    unsigned duration: 24;
} Effect;

typedef struct Effects {
    Effect slowness;
    Effect invulnerability;
    Effect regeneration;
    Effect degeneration;
    Effect invisibility;
    Effect freezeTime;
} Effects;

typedef struct Player {
    int x;
    int y;

    int lastX;
    int lastY;

    int chunkX;
    int chunkY;

    Direction dir;
    Effects effects;

    int life;
    int totalFragment;
    int biomeFragment;
    bool fragmentByCoins;
    unsigned short biomeCoins;
    unsigned short totalCoins;
    
    unsigned biome : 2;
    unsigned damaged: 1;
    float batery;

    CellType cellType;

    bool (*updateChunk)(struct Player*);
    void (*updateDirection)(struct Player*);
    void (*free)(struct Player*);
} Player;

Player* new_Player(int x, int y);

#endif