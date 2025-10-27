#ifndef MAP_H
#define MAP_H

#include "player.h"
#include "hashtable.h"
#include "linkedlist.h"

typedef struct {
    bool isWall;
    bool hasEnemy;
    unsigned int biomeType;
} Cell;

typedef struct Map {
    unsigned int rows;
    unsigned int cols;
    Cell **matrix;
    int **dist;

    HashTable* chunks;
    int chunkRows;
    int chunkCols;
    int chunkSize;

    Player* player;

    void (*update)(struct Map*, LinkedList*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(unsigned int rows, unsigned int cols, unsigned int chunkSize);

#endif