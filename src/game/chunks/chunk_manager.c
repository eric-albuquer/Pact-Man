#include "chunk_manager.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

const int CLOSER_IDX[9] = {16, 17, 18, 23, 24, 25, 30, 31, 32};
const int ADJACENT_IDX[8] = {16, 17, 18, 23, 25, 30, 31, 32};

static void loadAdjacents(ChunkManager* this){
    int cx = this->player->chunkX;
    int cy = this->player->chunkY;
    ChunkMap* chunks = this->chunks;
    ChunkLoader *cl = this->chunkLoader;
    int idx = 0;
    for (int i = -3; i < 4; i++){
        int chunkY = cy + i;
        for (int j = -3; j < 4; j++){
            int chunkX = cx + j;
            Chunk* chunk = chunks->get(chunks, chunkX, chunkY);
            if (chunkX >= 0 && chunkX < this->cols && chunkY >= 0 && chunkY < this->rows && !chunk && abs(i) < 2 && abs(j) < 2){
                chunk = new_Chunk(chunkX, chunkY);
                cl->generate(cl, chunk);
                chunks->add(chunks, chunk);
            }
            this->adjacents[idx++] = chunk;
        }
    }
}

static void updateChunks(ChunkManager* this){
    for (int i = 0; i < 9; i++){
        int idx = CLOSER_IDX[i];
        Chunk* chunk = this->adjacents[idx];
        if (!chunk) continue;
        chunk->update(chunk, this->degenerated);
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
    return &cells[(x & CHUNK_MASK) | ((y & CHUNK_MASK) << CHUNK_SHIFT)];
}

static Cell* getUpdatedCell(ChunkManager* this, int x, int y){
    if (x < 0 || y < 0) return NULL;
    int cx = (x >> CHUNK_SHIFT) - this->player->chunkX + 3;
    int cy = (y >> CHUNK_SHIFT) - this->player->chunkY + 3;

    if (cx < 2 || cx > 4 || cy < 2 || cy > 4) return NULL;
    Chunk* chunk = this->adjacents[cy * 7 + cx];
    if (chunk == NULL) return NULL;
    Cell* cells = chunk->cells;
    return &cells[(x & CHUNK_MASK) | ((y & CHUNK_MASK) << CHUNK_SHIFT)];
}

static Chunk* getChunk(ChunkManager* this, int cx, int cy){
    return this->chunks->get(this->chunks, cx, cy);
}

static Chunk* getLoadedChunk(ChunkManager* this, int cx, int cy){
    int rcx = cx - this->player->chunkX + 3;
    int rcy = cy - this->player->chunkY + 3;

    if (rcx < 0 || rcx > 6 || rcy < 0 || rcy > 6) return NULL;
    return this->adjacents[rcy * 7 + rcx];
}

static void _free(ChunkManager* this) {
    this->chunks->free(this->chunks);
    this->chunkLoader->free(this->chunkLoader);
    free(this);
}

ChunkManager* new_ChunkManager(int biomeCols, int rows, Player* p) {
    biomeCols++;
    ChunkManager* this = malloc(sizeof(ChunkManager));
    this->chunks = new_ChunkMap();
    this->cols = (biomeCols << 2) - 1;
    this->rows = rows;
    this->chunkLoader = new_ChunkLoader(biomeCols, rows, time(NULL));
    
    this->player = p;

    this->degenerated = -1;

    this->player->updateChunk(this->player);

    loadAdjacents(this);

    this->player->biome = getUpdatedCell(this, p->x, p->y)->biome;

    this->getChunk = getChunk;
    this->getLoadedChunk = getLoadedChunk;
    this->getLoadedCell = getLoadedCell;
    this->getUpdatedCell = getUpdatedCell;
    this->loadAdjacents = loadAdjacents;
    this->updateChunks = updateChunks;
    this->free = _free;
    return this;
}