#ifndef CHUNK_H
#define CHUNK_H

#include "linkedlist.h"
#include <stdbool.h>

#define MAX_ENIMIES_PER_CHUNK 2
#define CHUNK_SIZE 16
#define CELLS_PER_CHUNK (CHUNK_SIZE * CHUNK_SIZE)

typedef struct {
    bool isWall;
    int biomeType;
    int distance;
} Cell;

typedef struct Chunk {
    int x;
    int y;
    LinkedList* enemies;
    Cell* cells;

    void (*resetDistance)(struct Chunk*);
    void (*free)(struct Chunk*);
} Chunk;

Chunk* new_Chunk(int x, int y);

#endif