#ifndef MAP_H
#define MAP_H

#include "arraylist.h"
#include "controler.h"
#include "hashtable.h"
#include "linkedlist.h"
#include "player.h"
#include "chunk_manager.h"

#define FIRE_DAMAGE 10
#define SPIKE_DAMAGE 2 
#define ENEMY_DAGAME 15

#define COINS_TO_FRAGMENT 100

#define MUD_SLOWNESS_DURATION 3
#define SPIKE_SLOWNESS_DURATION 2 
#define FRUIT_INVULNERABILITY_DURATION 100

#define FONT_REGENERATION_STRENGTH 5
#define FONT_REGENERATION_DURATION 2

#define POTION_REGENERATION_STRENGTH 2
#define POTION_REGENERATION_DURATION 50

#define FREEZE_TIME_DURATION 90

#define DEGENERATION_DURATION 5
#define DEGENERATION_DAMAGE 1

#define INVISIBILITY_DURATION 100

#define MAX_PERSUIT_RADIUS 40
#define BEST_PATH_PROBABILITY 60

#define BOSS_FIRE_QUANTITY 60
#define BOSS_TENTACLE_QUANTITY 15

#define BOSS_BOMB_TIME 8.0f

#define PACMAN_KILL_COINS 10
#define PACMAN_KILL_HEALTH 20

// Constantes para ajudar no degen dos biomas
#define MAP_UPDATE_DT 0.15f
#define BIOME_DEGEN_START_TIME 180.0f

#define BATERY_DECAY 0.003f

typedef struct Map {
    ChunkManager* manager;
    ArrayList* changedChunk;

    LinkedList* firedCells;
    LinkedList* tentacleCells;

    float biomeTime;

    Player* player;
    unsigned int updateCount;

    void (*update)(struct Map*, Controler*);
    void (*restart)(struct Map*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(int chunkCols, int chunkRows, int spawnX, int spawnY);

#endif
