#ifndef CHUNKMAP_H
#define CHUNKMAP_H

#define CHUNK_MAP_LOAD_FACTOR 0.75f
#define CHUNK_MAP_START_CAPACITY 1024

#define EMPTY 0
#define OCCUPIED 1
#define REMOVED 2

#include "chunk.h"
#include <stdlib.h>

typedef struct ChunkMap {
    size_t *keys;
    Chunk** chunks;
    char* used;
    int mask;
    int size;
    int capacity;
    int maxFill;

    void (*add)(struct ChunkMap*, Chunk*);
    Chunk* (*get)(struct ChunkMap*, int, int);
    bool (*remove)(struct ChunkMap*, Chunk*);
    void (*free)(struct ChunkMap*);
} ChunkMap;

ChunkMap* new_ChunkMap();

#endif