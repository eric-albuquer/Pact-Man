#ifndef CHUNK_H
#define CHUNK_H

#include <stdbool.h>

#include "linkedlist.h"

#define CHUNK_SHIFT 4
#define CHUNK_SIZE 16
#define CHUNK_MASK 0xf
#define CELLS_PER_CHUNK 256

typedef enum {
    EMPTY,
    WALL,
    COIN,
    MUD,
    GRAVE,
    SPIKE,
    FIRE,
    FRAGMENT,
    WIND_RIGHT,
    WIND_LEFT,
    WIND_UP,
    WIND_DOWN
} CellType;

typedef struct {
    unsigned type : 4;
    unsigned biome : 2;

    signed distance : 9;
} Cell;

inline bool isWind(CellType type) {
    return type >= WIND_RIGHT && type <= WIND_DOWN;
}

typedef struct Chunk {
    int x;
    int y;

    unsigned isTransition : 1;
    unsigned isBorder : 1;
    unsigned biome : 2;
    unsigned isStructure : 1;
    unsigned isTemple : 1;
    unsigned isFont : 1;
    unsigned fragment : 1;

    unsigned int randCounter;

    LinkedList* enemies;
    Cell cells[CELLS_PER_CHUNK];

    void (*resetDistance)(struct Chunk*);
    void (*free)(struct Chunk*);
} Chunk;

Chunk* new_Chunk(int x, int y);

void setArgs(const int width, const int height, const int maxEnemiesPerChunk,
             const int seed);

#endif