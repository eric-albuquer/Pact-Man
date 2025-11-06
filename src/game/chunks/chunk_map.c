#include "chunk_map.h"
#include <stdlib.h>
#include <stdio.h>

static size_t chunkKey(int cx, int cy) {
    return ((size_t)(cy) << 32) | cx;
}

static int hashCode(size_t key) {
    return (int)(key ^ (key >> 32));
}

static void rehash(ChunkMap* map) {
    int oldCapacity = map->capacity;
    map->capacity <<= 1;
    map->mask = map->capacity - 1;
    map->maxFill = map->capacity * CHUNK_MAP_LOAD_FACTOR;
    size_t* newKeys = malloc(sizeof(size_t) * map->capacity);
    Chunk** newChunks = malloc(sizeof(Chunk*) * map->capacity);
    char* newUsed = calloc(map->capacity, sizeof(char));

    for (int i = 0; i < oldCapacity; i++) {
        if (map->used[i] == CHUNK_MAP_OCCUPIED) {
            size_t key = map->keys[i];
            Chunk* chunk = map->chunks[i];
            int newIdx = hashCode(key) & map->mask;
            while (newUsed[newIdx] == CHUNK_MAP_OCCUPIED) newIdx = (newIdx + 1) & map->mask;
            newKeys[newIdx] = key;
            newChunks[newIdx] = chunk;
            newUsed[newIdx] = CHUNK_MAP_OCCUPIED;
        }
    }
    free(map->keys);
    free(map->chunks);
    free(map->used);
    map->keys = newKeys;
    map->chunks = newChunks;
    map->used = newUsed;
}

static void add(ChunkMap* map, Chunk* chunk) {
    if (map->size >= map->maxFill) rehash(map);
    size_t key = chunkKey(chunk->x, chunk->y);
    int idx = hashCode(key) & map->mask;
    int firstRemoved = -1;
    while (map->used[idx] != EMPTY) {
        if (map->used[idx] == CHUNK_MAP_OCCUPIED && map->keys[idx] == key) return;
        if (map->used[idx] == CHUNK_MAP_REMOVED && firstRemoved == -1) firstRemoved = idx;
        idx = (idx + 1) & map->mask;
    }
    if (firstRemoved != -1) idx = firstRemoved;
    map->keys[idx] = key;
    map->chunks[idx] = chunk;
    map->used[idx] = CHUNK_MAP_OCCUPIED;
    map->size++;
}

static Chunk* get(ChunkMap* map, int cx, int cy) {
    size_t key = chunkKey(cx, cy);
    int idx = hashCode(key) & map->mask;
    while (map->used[idx] != EMPTY) {
        if (map->used[idx] == CHUNK_MAP_OCCUPIED && map->keys[idx] == key)
            return map->chunks[idx];
        idx = (idx + 1) & map->mask;
    }
    return NULL;
}

static bool delete(ChunkMap* map, Chunk* chunk) {
    size_t key = chunkKey(chunk->x, chunk->y);
    int idx = hashCode(key) & map->mask;
    while (map->used[idx] != EMPTY) {
        if (map->keys[idx] == key) {
            map->used[idx] = CHUNK_MAP_REMOVED;
            map->size--;
            return true;
        }
        idx = (idx + 1) & map->mask;
    }
    return false;
}

static void _free(ChunkMap* this){
    free(this->chunks);
    free(this->keys);
    free(this->used);
    free(this);
}

ChunkMap* new_ChunkMap() {
    ChunkMap* this = malloc(sizeof(ChunkMap));
    this->size = 0;
    this->capacity = CHUNK_MAP_START_CAPACITY;
    this->mask = this->capacity - 1;
    this->maxFill = this->capacity * CHUNK_MAP_LOAD_FACTOR;
    this->keys = malloc(sizeof(size_t) * this->capacity);
    this->chunks = malloc(sizeof(Chunk*) * this->capacity);
    this->used = calloc(this->capacity, sizeof(char));

    this->add = add;
    this->get = get;
    this->remove = delete;
    this->free = _free;
    return this;
}