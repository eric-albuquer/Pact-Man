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

static inline void updatePlayerBiome(Player* p, Cell* cell){
    if (cell->biome <= p->biome) return;
    p->biome = cell->biome;
    p->biomeCoins = 0;
    p->biomeFragment = 0;
    p->fragmentByCoins = false;
}

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
    } else if (cell->type == CELL_FRUIT) {
        cell->type = CELL_EMPTY;
    }
}

static inline void updatePlayerEffects(Player* p, Cell* cell) {
    static const int mudDuration = (MUD_SLOWNESS_DURATION << 1) - 1;
    static const int spikeDuration = (SPIKE_SLOWNESS_DURATION << 1) - 1;

    if (cell->type == CELL_MUD && p->effects.slowness.duration < mudDuration) {
        p->effects.slowness.duration = mudDuration;
        return;
    } else if (cell->type == CELL_SPIKE && p->effects.slowness.duration < spikeDuration) {
        p->effects.slowness.duration = spikeDuration;
        return;
    } else if (cell->type == CELL_FRUIT) {
        p->effects.invulnerability.duration = FRUIT_INVULNERABILITY_DURATION;
        return;
    }

    if (p->effects.slowness.duration > 0) {
        p->effects.slowness.duration--;
    }
    if (p->effects.invulnerability.duration > 0) {
        p->effects.invulnerability.duration--;
    }
}

static inline void updateDamagePlayer(Player* p, Cell* cell) {
    if (p->effects.invulnerability.duration > 0) return;
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

static inline Cell* movePlayer(ChunkManager* cm, Player* p, Vec2i newDir) {
    int playerNextX = p->x + newDir.x;
    int playerNextY = p->y + newDir.y;

    Cell* nextCell = cm->getUpdatedCell(cm, playerNextX, playerNextY);
    if (nextCell == NULL) return NULL;

    bool cantPassBiome = p->biomeFragment < 2 && nextCell->biome > p->biome;
    if (!isPassable(nextCell->type) || cantPassBiome || nextCell->biome < p->biome) return NULL;

    p->x = playerNextX;
    p->y = playerNextY;

    updatePlayerBiome(p, nextCell);

    p->updateDirection(p);

    if (p->updateChunk(p)) cm->loadAdjacents(cm);
    return nextCell;
}

static inline void updatePlayerWind(ChunkManager* cm, Player* p, Cell* cell) {
    if (isWind(cell->type)) {
        Vec2i dir = DIR_VECTOR[cell->type - CELL_WIND_RIGHT];
        movePlayer(cm, p, dir);
    }
}

static inline void updatePlayerByInput(ChunkManager* cm, Player* p, Cell* cell, Input input) {
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
}

static inline void updatePlayerMovement(ChunkManager* cm, Player* p, Cell* cell, Input input) {
    p->lastX = p->x;
    p->lastY = p->y;

    if ((p->effects.slowness.duration & 1) == 0)
        updatePlayerByInput(cm, p, cell, input);

    updatePlayerWind(cm, p, cell);
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DO PLAYER
//===============================================================

static inline void updatePlayer(ChunkManager* cm, Player* p, Input input) {
    Cell* cell = cm->getUpdatedCell(cm, p->x, p->y);

    updatePlayerEffects(p, cell);
    updatePlayerMovement(cm, p, cell, input);

    updatePlayerHealth(p, cell);
    updateDamagePlayer(p, cell);
    collectItens(p, cell);
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DOS INIMIGOS
//===============================================================

static inline void updateEnemyChunk(ChunkManager* cm, Node* node, LinkedList* list, ArrayList* changedChunk) {
    Enemy* e = node->data;
    if (!e->updateChunk(e)) return;
    e->changedChunk = true;
    list->removeNode(list, node);
    Chunk* newChunk = cm->getLoadedChunk(cm, e->chunkX, e->chunkY);
    newChunk->enemies->addLast(newChunk->enemies, e);
    changedChunk->push(changedChunk, e);
}

static inline void updateEnemyMovement(ChunkManager* cm, Node* node, LinkedList* list, Player* p) {
    Enemy* e = node->data;
    e->lastX = e->x;
    e->lastY = e->y;
    enemyStepTowardsPlayer(cm, e, p);
    e->updateDirection(e);
}

static inline bool checkPlayerEnemyColision(Node* node, LinkedList* enemies, Player* p) {
    Enemy* e = node->data;
    if ((e->lastX == p->x && e->lastY == p->y) || (e->x == p->x && e->y == p->y)) {
        if (p->effects.invulnerability.duration > 0) {
            e->free(e);
            enemies->removeNode(enemies, node);
            return true;
        } else {
            p->life -= ENEMY_DAGAME;
        }
    }
    return false;
}

static inline void updateEnemy(ChunkManager* cm, Node* node, LinkedList* list, Player* p, ArrayList* changedChunk) {
    if (checkPlayerEnemyColision(node, list, p)) return;
    updateEnemyMovement(cm, node, list, p);
    updateEnemyChunk(cm, node, list, changedChunk);
}

static inline void updateEnemies(Map* this) {
    Player* p = this->player;
    ChunkManager* cm = this->manager;

    ArrayList* changedChunk = this->changedChunk;
    changedChunk->clear(changedChunk);

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
            updateEnemy(cm, cur, enemies, p, changedChunk);
            cur = next;
        }
    }

    for (int i = 0; i < changedChunk->length; i++) {
        Enemy* e = changedChunk->data[i];
        e->changedChunk = false;
    }
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DO MAPA
//===============================================================

static void update(Map* this, Controler* controler) {
    ChunkManager* cm = this->manager;
    cm->updateChunks(cm);
    updatePlayer(cm, this->player, controler->input);
    updateEnemies(this);
    this->updateCount++;
}

static void _free(Map* this) {
    this->manager->free(this->manager);
    Player* p = this->player;
    p->free(p);
    this->changedChunk->free(this->changedChunk);
    free(this);
}

Map* new_Map(int chunkCols, int chunkRows) {
    Map* this = malloc(sizeof(Map));

    this->updateCount = 0;
    this->player = new_Player(201, 21);
    this->player->biome = 1;

    this->changedChunk = new_ArrayList();

    this->manager = new_ChunkManager(chunkCols, chunkRows, this->player);

    this->update = update;
    this->free = _free;
    return this;
}