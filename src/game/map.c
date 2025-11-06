#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chunk.h"
#include "enemy.h"
#include "pathfinding.h"
#include "chunk_loader.h"

typedef struct {
    int x;
    int y;
} Vec2i;

static const Vec2i DIR_VECTOR[4] = { {1, 0}, {-1, 0}, {0, -1}, {0, 1} };

static bool movePlayer(Map* this, Vec2i newDir) {
    Player* p = this->player;
    int playerNextX = p->x + newDir.x;
    int playerNextY = p->y + newDir.y;
    Cell* cell = this->manager->getLoadedCell(this->manager, playerNextX, playerNextY);
    if (cell == NULL || !isPassable(cell->type)) return false;

    p->x = playerNextX;
    p->y = playerNextY;

    p->updateDirection(p);

    p->biome = cell->biome;
    p->slowness = cell->type == CELL_MUD || cell->type == CELL_SPIKE;
    if (p->updateChunk(p)) this->manager->loadAdjacents(this->manager);
    return true;
}

static void updatePlayerWind(Map* this) {
    Player* p = this->player;

    Cell* floor = this->manager->getUpdatedCell(this->manager, p->x, p->y);
    if (isWind(floor->type)) {
        Vec2i dir = DIR_VECTOR[floor->type - CELL_WIND_RIGHT];
        movePlayer(this, dir);
    }
}

static void updatePlayer(Map* this, Input input) {
    Player* p = this->player;

    p->lastX = p->x;
    p->lastY = p->y;

    if (p->slowness) {
        p->slowness = 0;
        return;
    }

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
        if (movePlayer(this, dir[i])) break;
    }

    updatePlayerWind(this);
}

static void updateEnemies(Map* this) {
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
                    Chunk* newChunk =
                        cm->getLoadedChunk(cm, e->chunkX, e->chunkY);
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
    this->manager->updateAdjacents(this->manager);
    updatePlayer(this, controler->input);
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
    this->player = new_Player(301, 21);

    this->manager = new_ChunkManager(chunkCols, chunkRows, this->player);

    this->update = update;
    this->free = _free;
    return this;
}