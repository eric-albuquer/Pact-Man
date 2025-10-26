#ifndef MAP_H
#define MAP_H

#include "player.h"
#include "hashtable.h"

typedef struct {
    unsigned isWall : 1;
} Cell;

typedef struct Map {
    unsigned int rows;
    unsigned int cols;
    Cell **matrix;

    HashTable* chunks;

    Player* player;

    void (*update)(struct Map*, int, int);
    void (*free)(struct Map*);
} Map;

Map* new_Map(unsigned int rows, unsigned int cols, unsigned int chunkSize);

#endif