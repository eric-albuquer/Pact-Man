#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "common.h"
#include "chunk.h"

#define START_LIFE 100

#define START_SPEED \
((const int[DIFICULTY_COUNT]){ \
        150, \
        100, \
        50 \
}[dificulty])

typedef struct Effect {
    unsigned strenght : 8;
    unsigned int duration;
} Effect;

typedef struct Effects {
    Effect slowness;
    Effect invulnerability;
    Effect regeneration;
    Effect degeneration;
    Effect invisibility;
    Effect freezeTime;
    Effect speed;
} Effects;

typedef struct Player {
    int x;
    int y;

    int lastX;
    int lastY;

    int chunkX;
    int chunkY;

    int spawnX;
    int spawnY;

    Direction dir;
    Effects effects;

    int life;
    int speed;

    int totalFragment;
    int biomeFragment;
    bool fragmentByCoins;
    unsigned short biomeCoins;
    unsigned short totalCoins;

    float totalTime;

    unsigned biome : 2;
    unsigned damaged : 1;
    unsigned hitEnemy : 1;
    unsigned getFragment : 1;
    unsigned alive : 1;
    unsigned biomeChange: 1;

    float batery;

    CellType cellType;

    bool (*updateChunk)(struct Player*);
    void (*updateDirection)(struct Player*);
    void (*restart)(struct Player*);
    void (*free)(struct Player*);
} Player;

Player* new_Player(int x, int y);

#endif