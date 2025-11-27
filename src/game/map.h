#ifndef MAP_H
#define MAP_H

#include "arraylist.h"
#include "controller.h"
#include "linkedlist.h"
#include "player.h"
#include "chunk_manager.h"
#include "common.h"

typedef struct Map {
    ChunkManager* manager;
    ArrayList* changedChunk;

    LinkedList* firedCells;
    LinkedList* tentacleCells;

    float biomeTime;

    Player* player;
    Score lastScore;
    unsigned int updateCount;

    bool running;

    void (*update)(struct Map*, Controller*, float);
    void (*restart)(struct Map*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(int chunkCols, int chunkRows, int spawnX, int spawnY);

#endif
