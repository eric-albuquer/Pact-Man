#ifndef MAP_H
#define MAP_H

#include "arraylist.h"
#include "controler.h"
#include "hashtable.h"
#include "linkedlist.h"
#include "player.h"
#include "chunk_manager.h"

typedef struct Map {
    ChunkManager* manager;

    Player* player;
    bool changedChunk;

    void (*update)(struct Map*, Controler*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(int chunkCols, int chunkRows);

#endif