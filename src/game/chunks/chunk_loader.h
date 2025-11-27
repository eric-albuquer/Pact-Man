#ifndef CHUNK_LOADER_H
#define CHUNK_LOADER_H

#include "chunk.h"

#define BIOME_ENEMY_PROBABILITY (int[4]){ 50, 40, 20, 20 }

#define WALL_DENSITY 75

#define MUD_DENSITY 7

#define GRAVE_DENSITY 5
#define GRAVE_INFESTED_PROBABILITY 5

#define FIRE_DENSITY 30

#define SPIKE_DENSITY 5

#define COIN_DENSITY 15

#define FRUIT_PROBABILITY 8

#define INVISIBILITY_PROBABILITY 8

#define REGENERATION_PROBABILITY 8

#define BATERY_PROBABILITY 50

#define FREEZE_TIME_PROBABILITY 4

typedef struct ChunkLoader{
    int width, height;
    int biomeWidthChunks;
    int seed;

    void (*generate)(struct ChunkLoader*, Chunk*);
    void (*free)(struct ChunkLoader*);
} ChunkLoader;

ChunkLoader* new_ChunkLoader(const int biomeWidth, const int height, const int seed);

#endif