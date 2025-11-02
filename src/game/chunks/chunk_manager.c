#include "chunk_manager.h"
#include <stdlib.h>
#include <math.h>

const int CLOSER_IDX[9] = {16, 17, 18, 23, 24, 25, 30, 31, 32};

static void loadAdjacents(ChunkManager* this){
    int cx = this->player->chunkX;
    int cy = this->player->chunkY;
    ChunkMap* chunks = this->chunks;
    int idx = 0;
    for (int i = -3; i < 4; i++){
        int chunkY = cy + i;
        for (int j = -3; j < 4; j++){
            int chunkX = cx + j;
            Chunk* chunk = chunks->get(chunks, chunkX, chunkY);
            if (chunkX >= 0 && chunkX < this->cols && chunkY >= 0 && chunkY < this->rows && !chunk && abs(i) < 2 && abs(j) < 2){
                chunk = new_Chunk(chunkX, chunkY, this->seed);
                chunks->add(chunks, chunk);
            }
            this->adjacents[idx++] = chunk;
        }
    }
}

static Cell* getLoadedCell(ChunkManager* this, int x, int y){
    if (x < 0 || y < 0) return NULL;
    int cx = (x >> CHUNK_SHIFT) - this->player->chunkX + 3;
    int cy = (y >> CHUNK_SHIFT) - this->player->chunkY + 3;

    if (cx < 0 || cx > 6 || cy < 0 || cy > 6) return NULL;
    Chunk* chunk = this->adjacents[cy * 7 + cx];
    if (chunk == NULL) return NULL;
    Cell* cells = chunk->cells;
    return &cells[(x & CHUNK_MASK) + ((y & CHUNK_MASK) << CHUNK_SHIFT)];
}

static Cell* getUpdatedCell(ChunkManager* this, int x, int y){
    if (x < 0 || y < 0) return NULL;
    int cx = (x >> CHUNK_SHIFT) - this->player->chunkX + 3;
    int cy = (y >> CHUNK_SHIFT) - this->player->chunkY + 3;

    if (cx < 2 || cx > 4 || cy < 2 || cy > 4) return NULL;
    Chunk* chunk = this->adjacents[cy * 7 + cx];
    if (chunk == NULL) return NULL;
    Cell* cells = chunk->cells;
    return &cells[(x & CHUNK_MASK) + ((y & CHUNK_MASK) << CHUNK_SHIFT)];
}

static Chunk* getChunk(ChunkManager* this, int cx, int cy){
    return this->chunks->get(this->chunks, cx, cy);
}

static void _free(ChunkManager* this) {
    this->chunks->free(this->chunks);
    free(this);
}

ChunkManager* new_ChunkManager(int cols, int rows, Player* p, int seed) {
    ChunkManager* this = malloc(sizeof(ChunkManager));
    this->chunks = new_ChunkMap();
    this->changedChunk = false;
    this->cols = cols;
    this->rows = rows;
    this->player = p;
    this->seed = seed;

    this->player->updateChunk(this->player);

    loadAdjacents(this);

    this->getChunk = getChunk;
    this->getLoadedCell = getLoadedCell;
    this->getUpdatedCell = getUpdatedCell;
    this->loadAdjacents = loadAdjacents;
    this->free = _free;
    return this;
}