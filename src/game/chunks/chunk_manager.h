#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include "chunk_map.h"
#include "player.h"
#include "chunk_loader.h"

extern const int CLOSER_IDX[9];

typedef struct ChunkManager {
    int rows;
    int cols;

    Player* player;
    ChunkMap* chunks;
    ChunkLoader* chunkLoader;
    Chunk* adjacents[49];
    int degenerated;

    Chunk* (*getLoadedChunk)(struct ChunkManager*, int, int);
    Chunk* (*getChunk)(struct ChunkManager*, int, int);
    Cell* (*getLoadedCell)(struct ChunkManager*, int, int);
    Cell* (*getUpdatedCell)(struct ChunkManager*, int, int);
    void (*updateChunks)(struct ChunkManager*);
    void (*loadAdjacents)(struct ChunkManager*);
    void (*free)(struct ChunkManager*);
} ChunkManager;

ChunkManager* new_ChunkManager(int biomeCols, int rows, Player* p);

#endif