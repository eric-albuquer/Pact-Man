#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "chunk_map.h"
#include "player.h"

extern const int CLOSER_IDX[9];

typedef struct ChunkManager {
    int rows;
    int cols;

    Player* player;
    ChunkMap* chunks;
    Chunk* adjacents[49];
    bool changedChunk;

    Chunk* (*getLoadedChunk)(struct ChunkManager*, int, int);
    Chunk* (*getChunk)(struct ChunkManager*, int, int);
    Cell* (*getLoadedCell)(struct ChunkManager*, int, int);
    Cell* (*getUpdatedCell)(struct ChunkManager*, int, int);
    void (*loadAdjacents)(struct ChunkManager*);
    void (*free)(struct ChunkManager*);
} ChunkManager;

ChunkManager* new_ChunkManager(int cols, int rows, Player* p);

#endif