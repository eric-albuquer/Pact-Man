#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "enemy.h"
#include "pathfinding.h"

typedef struct {
    int x;
    int y;
} VecInt2;

static bool movePlayer(Map* this, VecInt2 newDir) {
    Player* p = this->player;
    int playerNextX = p->x + newDir.x;
    int playerNextY = p->y + newDir.y;
    Cell* cell =
        this->manager->getLoadedCell(this->manager, playerNextX, playerNextY);
    if (cell == NULL || cell->isWall) return false;

    p->x = playerNextX;
    p->y = playerNextY;
    if (p->updateChunk(p)) this->changedChunk = true;
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
    if (this->changedChunk) this->manager->loadAdjacents(this->manager);
    updateEnemies(this);
}

static void _free(Map* this) {
    this->manager->free(this->manager);
    Player* p = this->player;
    p->free(p);
    free(this);
}

Map* new_Map(int chunkCols, int chunkRows) {
    Map* this = malloc(sizeof(Map));
    
    this->player = new_Player(11, 11);

    this->manager = new_ChunkManager(chunkCols, chunkRows, this->player);

    this->update = update;
    this->free = _free;
    return this;
}