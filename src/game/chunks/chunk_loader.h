#ifndef CHUNK_LOADER_H
#define CHUNK_LOADER_H

#include "chunk.h"

#define BIOME_ENEMY_PROBABILITY (int[4]){ 50, 50, 20, 20 }

#define WALL_DENSITY 70

#define MUD_DENSITY 10

#define GRAVE_DENSITY 5
#define GRAVE_INFESTED_PROBABILITY 5

#define FIRE_DENSITY 30

#define SPIKE_DENSITY 5

#define COIN_DENSITY 50

#define FRUIT_PROBABILITY 8

typedef struct ChunkLoader{
    int width, height;
    int biomeWidthChunks;
    int seed;

    void (*generate)(struct ChunkLoader*, Chunk*);
    void (*free)(struct ChunkLoader*);
} ChunkLoader;

ChunkLoader* new_ChunkLoader(const int biomeWidth, const int height, const int seed);

#endif