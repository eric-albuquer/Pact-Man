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
#define FONT_HEALTH 1

#define COINS_TO_FRAGMENT 100

#define MUD_SLOWNESS_DURATION 3
#define SPIKE_SLOWNESS_DURATION 5
#define FRUIT_INVULNERABILITY_DURATION 100

typedef struct Map {
    ChunkManager* manager;
    ArrayList* changedChunk;

    float degenerescence;

    Player* player;
    unsigned int updateCount;

    void (*update)(struct Map*, Controler*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(int chunkCols, int chunkRows);

#endif