#ifndef MAP_H
#define MAP_H

#include "arraylist.h"
#include "controler.h"
#include "hashtable.h"
#include "linkedlist.h"
#include "player.h"

typedef struct Map {
    Chunk* nearChunks[9];
    HashTable* chunks;

    int chunkRows;
    int chunkCols;

    Player* player;
    bool changedChunk;

    Cell* (*getLoadedCell)(struct Map*, int, int);
    Chunk* (*getChunk)(struct Map*, int, int);
    void (*update)(struct Map*, Controler*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(int chunkCols, int chunkRows);

#endif