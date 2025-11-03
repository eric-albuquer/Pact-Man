#ifndef CHUNK_H
#define CHUNK_H

#include "linkedlist.h"
#include <stdbool.h>

#define MAX_ENIMIES_PER_CHUNK 2
#define CHUNK_SHIFT 4
#define CHUNK_SIZE 16
#define CHUNK_MASK 0xf
#define CELLS_PER_CHUNK 256

#define BIOME_WIDTH_CHUNKS 5
#define HALF_WIDTH_CHUNKS (BIOME_WIDTH_CHUNKS >> 1)

#define SEED 2312512515

typedef struct {
    unsigned isWall : 1;
    unsigned isBoss : 1;
    int biomeType;
    int distance;
} Cell;

typedef struct Chunk {
    int x;
    int y;

    LinkedList* enemies;
    Cell cells[CELLS_PER_CHUNK];

    void (*resetDistance)(struct Chunk*);
    void (*free)(struct Chunk*);
} Chunk;

Chunk* new_Chunk(int x, int y);

#endif