#ifndef CHUNK_LOADER_H
#define CHUNK_LOADER_H

#include "chunk.h"

typedef struct ChunkLoader{
    int width, height;
    int biomeWidthChunks;
    int seed;

    void (*generate)(struct ChunkLoader*, Chunk*);
    void (*free)(struct ChunkLoader*);
} ChunkLoader;

ChunkLoader* new_ChunkLoader(const int biomeWidth, const int width, const int height, const int seed);

#endif