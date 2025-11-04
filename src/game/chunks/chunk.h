#ifndef CHUNK_H
#define CHUNK_H

#include "linkedlist.h"
#include <stdbool.h>

#define CHUNK_SHIFT 4
#define CHUNK_SIZE 16
#define CHUNK_MASK 0xf
#define CELLS_PER_CHUNK 256

typedef struct {
    unsigned isWall : 2;
    unsigned isBossTemple : 1;
    unsigned biomeType: 5;
    signed distance: 9;
    unsigned windDir : 3;
    unsigned coin : 1;
} Cell;

typedef struct Chunk {
    int x;
    int y;

    unsigned isTransition : 1;
    unsigned isBorder: 1;
    unsigned biome: 2;
    unsigned isTemple: 1;

    LinkedList* enemies;
    Cell cells[CELLS_PER_CHUNK];

    void (*resetDistance)(struct Chunk*);
    void (*free)(struct Chunk*);
} Chunk;

Chunk* new_Chunk(int x, int y);

void setArgs(const int width, const int height, const int maxEnemiesPerChunk, const int seed);

#endif