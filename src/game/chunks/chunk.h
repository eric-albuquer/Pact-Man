#ifndef CHUNK_H
#define CHUNK_H

#include <stdbool.h>
#include "enemy.h"
#include "linkedlist.h"

#define CHUNK_SHIFT 4
#define CHUNK_SIZE 16
#define CHUNK_MASK 0xf
#define CELLS_PER_CHUNK 256

typedef enum {
    CELL_EMPTY,
    CELL_WALL,
    CELL_COIN,
    CELL_MUD,
    CELL_GRAVE,
    CELL_GRAVE_INFESTED,
    CELL_SPIKE,
    CELL_FIRE_OFF,
    CELL_FIRE_ON,
    CELL_FRAGMENT,
    CELL_FRUIT,
    CELL_WIND_RIGHT,
    CELL_WIND_LEFT,
    CELL_WIND_UP,
    CELL_WIND_DOWN
} CellType;

typedef struct {
    unsigned type : 4;
    unsigned biome : 2;

    signed distance : 9;
} Cell;

inline bool isWind(CellType type) {
    return type >= CELL_WIND_RIGHT && type <= CELL_WIND_DOWN;
}

inline bool isPassable(CellType type) {
    return type != CELL_WALL && type != CELL_GRAVE && type != CELL_GRAVE_INFESTED;
}

inline bool isSafe(CellType type) {
    return type != CELL_FIRE_ON && type != CELL_SPIKE;
}

typedef enum {
    CHUNK_NORMAL,
    CHUNK_TRANSITION,
    CHUNK_TEMPLE,
    CHUNK_FONT,
    CHUNK_FRAGMENT,
} ChunkType;

inline bool isStructure(ChunkType type) {
    return type >= CHUNK_TEMPLE && type <= CHUNK_FONT;
}

typedef struct Chunk {
    int x;
    int y;

    unsigned type : 3;
    unsigned biome : 2;
    unsigned isBorder : 1;

    unsigned int randCounter;

    LinkedList* enemies;
    Cell cells[CELLS_PER_CHUNK];

    void (*update)(struct Chunk*);
    void (*resetDistance)(struct Chunk*);
    void (*free)(struct Chunk*);
} Chunk;

Chunk* new_Chunk(int x, int y);

void setArgs(const int width, const int height, const int seed);

#endif