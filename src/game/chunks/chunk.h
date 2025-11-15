#ifndef CHUNK_H
#define CHUNK_H

#include <stdbool.h>
#include "enemy.h"
#include "linkedlist.h"
#include "common.h"

#define CHUNK_SHIFT 4
#define CHUNK_SIZE 16
#define CHUNK_MASK 0xf
#define CELLS_PER_CHUNK 256
#define TOTAL_CELLS_MASK 0xff

#define DEGENERATION_PROBABILITY 50

typedef enum {
    CELL_EMPTY,
    CELL_FONT_HEALTH,
    CELL_TEMPLE,
    CELL_WALL,
    CELL_COIN,
    CELL_MUD,
    CELL_GRAVE,
    CELL_GRAVE_INFESTED,
    CELL_SPIKE,
    CELL_FIRE_OFF,
    CELL_FIRE_ON,
    CELL_TENTACLE,
    CELL_FRAGMENT,
    CELL_FRUIT,
    CELL_INVISIBILITY,
    CELL_REGENERATION,
    CELL_BATERY,
    CELL_FREEZE_TIME,
    CELL_WIND_RIGHT,
    CELL_WIND_LEFT,
    CELL_WIND_UP,
    CELL_WIND_DOWN,
    CELL_DEGENERATED_1,
    CELL_DEGENERATED_2,
    CELL_DEGENERATED_3,
    CELL_HEAVEN,
    CELL_PORTAL,
} CellType;

inline bool isDegenerated(CellType type){
    return type >= CELL_DEGENERATED_1 && type <= CELL_DEGENERATED_3;
}

inline bool isWind(CellType type) {
    return type >= CELL_WIND_RIGHT && type <= CELL_WIND_DOWN;
}

inline bool isPassable(CellType type) {
    return type != CELL_WALL && type != CELL_GRAVE && type != CELL_GRAVE_INFESTED;
}

// inline bool isSafe(CellType type) {
//     return type != CELL_FIRE_ON && type != CELL_SPIKE;
// }

typedef struct {
    unsigned type : 5;
    unsigned biome : 2;

    signed distance : 9;
} Cell;

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

    void (*update)(struct Chunk*, int, bool);
    void (*resetDistance)(struct Chunk*);
    Cell* (*cellAt)(struct Chunk*, int, int);
    void (*free)(struct Chunk*);
} Chunk;

Chunk* new_Chunk(int x, int y);

#endif