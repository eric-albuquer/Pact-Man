#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chunk.h"
#include "enemy.h"
#include "pathfinding.h"
#include "chunk_loader.h"

//===============================================================
//  FUNÇÕES AUXILIARES DA ATUALIZAÇÃO DO PLAYER
//===============================================================

static inline void collectItens(Player* p, Cell* cell) {
    if (cell->type == CELL_COIN) {
        cell->type = CELL_EMPTY;
        p->biomeCoins++;
        p->totalCoins++;
        if (p->fragmentByCoins == false && p->biomeCoins / COINS_TO_FRAGMENT) {
            p->fragmentByCoins = true;
            p->biomeFragment++;
            p->totalFragment++;
        }
    } else if (cell->type == CELL_FRAGMENT) {
        cell->type = CELL_EMPTY;
        p->totalFragment++;
        p->biomeFragment++;
    }
}

static inline void updatePlayerEffects(Player* p, Cell* cell) {
    if (p->effects.slowness) {
        p->effects.slowness = 0;
        return;
    }
    p->effects.slowness = cell->type == CELL_MUD || cell->type == CELL_SPIKE;
}

static inline void updateDamagePlayer(Player* p, Cell* cell) {
    if (cell->type == CELL_FIRE_ON) {
        p->life -= FIRE_DAMAGE;
    } else if (cell->type == CELL_SPIKE) {
        p->life -= SPIKE_DAMAGE;
    }
}

static inline void updatePlayerHealth(Player* p, Cell* cell) {
    if (cell->type == CELL_FONT_HEALTH && p->life < START_LIFE) {
        p->life += FONT_HEALTH;
    }
}

//===============================================================
//  FUNÇÕES DE MOVIMENTAÇÃO DO PLAYER
//===============================================================

typedef struct {
    int x;
    int y;
} Vec2i;

static const Vec2i DIR_VECTOR[4] = {
    {1, 0},
    {-1, 0},
    {0, -1},
    {0, 1}
};

static Cell* movePlayer(ChunkManager* cm, Player* p, Vec2i newDir) {
    int playerNextX = p->x + newDir.x;
    int playerNextY = p->y + newDir.y;

    Cell* nextCell = cm->getUpdatedCell(cm, playerNextX, playerNextY);
    if (nextCell == NULL) return NULL;

    bool cantPassBiome = p->biomeFragment < 2 && nextCell->biome > p->biome;
    if (!isPassable(nextCell->type) || cantPassBiome || nextCell->biome < p->biome) return NULL;

    p->x = playerNextX;
    p->y = playerNextY;

    p->updateDirection(p);

    if (p->updateChunk(p)) cm->loadAdjacents(cm);
    return nextCell;
}

static void updatePlayerWind(ChunkManager* cm, Player* p, Cell* cell) {
    if (isWind(cell->type)) {
        Vec2i dir = DIR_VECTOR[cell->type - CELL_WIND_RIGHT];
        movePlayer(cm, p, dir);
    }
}

static void updatePlayerMovment(ChunkManager* cm, Player* p, Cell* cell, Input input) {
    p->lastX = p->x;
    p->lastY = p->y;

    static Vec2i dir[4];
    int length = 0;

    if (p->dir == RIGHT) {
        if (input.left) dir[length++] = (Vec2i){ -1, 0 };
        if (input.up) dir[length++] = (Vec2i){ 0, -1 };
        if (input.down) dir[length++] = (Vec2i){ 0, 1 };
        if (input.right) dir[length++] = (Vec2i){ 1, 0 };
    } else if (p->dir == LEFT) {
        if (input.right) dir[length++] = (Vec2i){ 1, 0 };
        if (input.up) dir[length++] = (Vec2i){ 0, -1 };
        if (input.down) dir[length++] = (Vec2i){ 0, 1 };
        if (input.left) dir[length++] = (Vec2i){ -1, 0 };
    } else if (p->dir == UP) {
        if (input.down) dir[length++] = (Vec2i){ 0, 1 };
        if (input.right) dir[length++] = (Vec2i){ 1, 0 };
        if (input.left) dir[length++] = (Vec2i){ -1, 0 };
        if (input.up) dir[length++] = (Vec2i){ 0, -1 };
    } else if (p->dir == DOWN) {
        if (input.up) dir[length++] = (Vec2i){ 0, -1 };
        if (input.right) dir[length++] = (Vec2i){ 1, 0 };
        if (input.left) dir[length++] = (Vec2i){ -1, 0 };
        if (input.down) dir[length++] = (Vec2i){ 0, 1 };
    }

    for (int i = 0; i < length; i++) {
        Cell* nextCell = movePlayer(cm, p, dir[i]);
        if (nextCell) {
            cell = nextCell;
            break;
        }
    }

    updatePlayerWind(cm, p, cell);
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DO PLAYER
//===============================================================

static void updatePlayer(ChunkManager* cm, Player* p, Input input) {
    Cell* cell = cm->getUpdatedCell(cm, p->x, p->y);
    updatePlayerMovment(cm, p, cell, input);

    updatePlayerHealth(p, cell);
    updateDamagePlayer(p, cell);
    updatePlayerEffects(p, cell);
    collectItens(p, cell);
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DOS INIMIGOS
//===============================================================

static void updateEnemies(Map* this) {
    Player* p = this->player;
    ChunkManager* cm = this->manager;
    static Enemy* changed[100];
    int changedLength = 0;

    mapDistancePlayer(this);

    Chunk** adjacents = this->manager->adjacents;

    for (int i = 0; i < 9; i++) {
        int idx = CLOSER_IDX[i];
        Chunk* chunk = adjacents[idx];
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
                if (e->updateChunk(e)) {
                    e->changedChunk = true;
                    changed[changedLength++] = e;
                    enemies->removeNode(enemies, cur);
                    Chunk* newChunk = cm->getLoadedChunk(cm, e->chunkX, e->chunkY);
                    LinkedList* newEnemies = newChunk->enemies;
                    newEnemies->addLast(newEnemies, e);
                }
                if (e->x == p->x && e->y == p->y) {
                    p->life -= ENEMY_DAGAME;
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

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DO MAPA
//===============================================================

static void update(Map* this, Controler* controler) {
    ChunkManager* cm = this->manager;
    cm->updateAdjacentsChunks(cm);
    updatePlayer(cm, this->player, controler->input);
    updateEnemies(this);
    this->updateCount++;
}

static void _free(Map* this) {
    this->manager->free(this->manager);
    Player* p = this->player;
    p->free(p);
    free(this);
}

Map* new_Map(int chunkCols, int chunkRows) {
    Map* this = malloc(sizeof(Map));

    this->updateCount = 0;
    this->player = new_Player(11, 21);

    this->manager = new_ChunkManager(chunkCols, chunkRows, this->player);

    this->update = update;
    this->free = _free;
    return this;
}