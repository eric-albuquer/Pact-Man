#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "enemy.h"
#include "pathfinding.h"

static char key[100];

static Cell* getLoadedCell(Map* this, int x, int y) {
    if (x < 0 || y < 0) return NULL;
    int chunkX = (x / CHUNK_SIZE) - this->player->chunkX + 1;
    int chunkY = (y / CHUNK_SIZE) - this->player->chunkY + 1;

    if (chunkX < 0 || chunkX > 2 || chunkY < 0 || chunkY > 2) return NULL;

    Chunk* chunk = this->nearChunks[chunkY * 3 + chunkX];
    if (chunk == NULL) return NULL;
    Cell* cells = chunk->cells;
    return &cells[(x % CHUNK_SIZE) + (y % CHUNK_SIZE) * CHUNK_SIZE];
}

static Chunk* getChunk(Map* this, int x, int y) {
    if (x < 0 || x >= this->chunkCols || y < 0 || y > this->chunkRows)
        return NULL;
    sprintf(key, "%d,%d", y, x);
    return this->chunks->get(this->chunks, key);
}

static void loadChunks(Map* this) {
    Player* p = this->player;
    HashTable* chunks = this->chunks;
    Chunk** nearChunks = this->nearChunks;
    int idx = 0;
    for (int i = -1; i < 2; i++) {
        int cy = i + p->chunkY;
        for (int j = -1; j < 2; j++) {
            int cx = j + p->chunkX;
            Chunk* chunk = getChunk(this, cx, cy);
            if (cx >= 0 && cx < this->chunkCols && cy >= 0 &&
                cy < this->chunkRows && !chunk) {
                chunk = new_Chunk(cx, cy);
                chunks->set(chunks, key, chunk);
            }
            nearChunks[idx++] = chunk;
        }
    }
}

typedef struct {
    int x;
    int y;
} VecInt2;

static bool movePlayer(Map* this, VecInt2 newDir) {
    Player* p = this->player;
    int playerNextX = p->x + newDir.x;
    int playerNextY = p->y + newDir.y;
    Cell* cell = this->getLoadedCell(this, playerNextX, playerNextY);
    if (cell == NULL || cell->isWall) return false;

    p->x = playerNextX;
    p->y = playerNextY;
    if (p->updateChunk(p, CHUNK_SIZE)) this->changedChunk = true;
    p->updateDirection(p);
    return true;
}

static void updatePlayer(Map* this, Input input) {
    Player* p = this->player;

    static VecInt2 dir[4];
    int length = 0;

    if (p->dir == RIGHT) {
        if (input.left) dir[length++] = (VecInt2){-1, 0};
        if (input.up) dir[length++] = (VecInt2){0, -1};
        if (input.down) dir[length++] = (VecInt2){0, 1};
        if (input.right) dir[length++] = (VecInt2){1, 0};
    } else if (p->dir == LEFT) {
        if (input.right) dir[length++] = (VecInt2){1, 0};
        if (input.up) dir[length++] = (VecInt2){0, -1};
        if (input.down) dir[length++] = (VecInt2){0, 1};
        if (input.left) dir[length++] = (VecInt2){-1, 0};
    } else if (p->dir == UP) {
        if (input.down) dir[length++] = (VecInt2){0, 1};
        if (input.right) dir[length++] = (VecInt2){1, 0};
        if (input.left) dir[length++] = (VecInt2){-1, 0};
        if (input.up) dir[length++] = (VecInt2){0, -1};
    } else if (p->dir == DOWN) {
        if (input.up) dir[length++] = (VecInt2){0, -1};
        if (input.right) dir[length++] = (VecInt2){1, 0};
        if (input.left) dir[length++] = (VecInt2){-1, 0};
        if (input.down) dir[length++] = (VecInt2){0, 1};
    }

    p->lastX = p->x;
    p->lastY = p->y;

    for (int i = 0; i < length; i++) {
        if (movePlayer(this, dir[i])) break;
    }
}

static void updateEnemies(Map* this) {
    static Enemy* changed[100];
    int changedLength = 0;

    mapDistancePlayer(this);

    HashTable* chunks = this->chunks;
    Chunk** nearChunks = this->nearChunks;

    for (int i = 0; i < 9; i++) {
        Chunk* chunk = nearChunks[i];
        if (!chunk) continue;
        LinkedList* enemies = chunk->enemies;
        Node* cur = enemies->head;
        while (cur != NULL) {
            Node* next = cur->next;
            Enemy* e = cur->data;
            if (e->changedChunk) break;
            e->lastX = e->x;
            e->lastY = e->y;
            if (enemyStepTowardsPlayer(this, e)) {
                if (e->updateChunk(e, CHUNK_SIZE)) {
                    e->changedChunk = true;
                    changed[changedLength++] = e;
                    enemies->removeNode(enemies, cur);
                    Chunk* newChunk = getChunk(this, e->chunkX, e->chunkY);
                    if (!newChunk) {
                        newChunk = new_Chunk(e->chunkX, e->chunkY);
                        chunks->set(chunks, key, newChunk);
                    }
                    LinkedList* newEnemies = newChunk->enemies;
                    newEnemies->addLast(newEnemies, e);
                }
                e->updateDirection(e);
            }
            cur = next;
        }
    }

    for (int i = 0; i < changedLength; i++) {
        Enemy* e = changed[i];
        e->changedChunk = false;
    }
}

static void update(Map* this, Controler* controler) {
    this->changedChunk = false;
    updatePlayer(this, controler->input);
    if (this->changedChunk) loadChunks(this);
    updateEnemies(this);
}

static void _free(Map* this) {
    HashNode* cur = this->chunks->keys->head;
    while (cur != NULL) {
        Chunk* chunk = cur->data;
        chunk->free(chunk);
        cur = cur->nextKey;
    }
    HashTable* h = this->chunks;
    h->free(h);

    Player* p = this->player;
    p->free(p);
    free(this);
}

Map* new_Map(int chunkCols, int chunkRows) {
    Map* this = malloc(sizeof(Map));
    this->chunkCols = chunkCols;
    this->chunkRows = chunkRows;
    this->chunks = new_HashTable(chunkCols * chunkRows);

    this->player = new_Player(11, 11);
    this->player->updateChunk(this->player, CHUNK_SIZE);
    loadChunks(this);
    this->changedChunk = false;

    this->getLoadedCell = getLoadedCell;
    this->getChunk = getChunk;
    this->update = update;
    this->free = _free;
    return this;
}