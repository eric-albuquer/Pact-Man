#ifndef MAP_H
#define MAP_H

#include "player.h"
#include "hashtable.h"
#include "linkedlist.h"
#include "arraylist.h"
#include "controler.h"

typedef struct {
    bool isWall;
    unsigned int biomeType;
    int distance;
} Cell;

typedef struct Map {
    unsigned int rows;
    unsigned int cols;
    Cell **matrix;

    ArrayList* nearEnemies;
    HashTable* chunks;
    int chunkRows;
    int chunkCols;
    int chunkSize;

    Player* player;

    void (*update)(struct Map*, Controler*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(unsigned int rows, unsigned int cols, unsigned int chunkSize);

#endif