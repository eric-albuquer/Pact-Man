#include "chunk.h"
#include <stdlib.h>
#include "enemy.h"

static inline Cell* cellAt(Chunk* this, int x, int y) {
    return &this->cells[(y << CHUNK_SHIFT) | x];
}

static void resetDistance(Chunk* this) {
    Cell* cells = this->cells;
    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        cells[i].distance = -1;
    }
}

static bool inline isFire(Cell* cell) {
    return cell->type == CELL_FIRE_OFF || cell->type == CELL_FIRE_ON;
}

static void updateDegenerescence(Chunk* this, int x, int y, int biome) {
    Cell* cell = cellAt(this, x, y);
    if (cell->type == CELL_FRAGMENT || rand() % 100 >= DEGENERATION_PROBABILITY) return;
    if (cell->biome > biome) return;
    if (!isDegenerated(cell->type))
        cell->type = CELL_DEGENERATED_1;
    else if (cell->type < CELL_DEGENERATED_3)
        cell->type++;
}

static void updateFire(Chunk* this, int x, int y) {
    if (this->biome != 2) return;
    Cell* cell = cellAt(this, x, y);
    if (!isFire(cell)) return;
    CellType switchFire = cell->type == CELL_FIRE_ON ? CELL_FIRE_OFF : CELL_FIRE_ON;
    for (int i = -1; i < 2; i++) {
        int nx = x + i;
        int ny = y + i;
        Cell* cell = cellAt(this, nx, y);
        if (nx >= 0 && nx < CHUNK_SIZE && isFire(cell)) {
            cell->type = switchFire;
        }
        if (i != 0 && ny >= 0 && ny < CHUNK_SIZE && isFire((cell = cellAt(this, x, ny)))) {
            cellAt(this, x, ny)->type = switchFire;
        }
    }
}

static Node* hasEnemyInCell(Chunk* this, int x, int y) {
    Node* cur = this->enemies->head;
    while (cur) {
        Enemy* e = cur->data;
        int ex = e->x & CHUNK_MASK;
        int ey = e->y & CHUNK_MASK;
        if (ex == x && ey == y) return cur;
        cur = cur->next;
    }
    return NULL;
}

static void updateGraveInfested(Chunk* this, int x, int y) {
    Cell* cell = cellAt(this, x, y);
    if (cell->biome < HERESIA) return;
    LinkedList* enemies = this->enemies;
    if (cell->type == CELL_GRAVE_INFESTED) {
        cell->type = CELL_EMPTY;
        int ex = x + (this->x << CHUNK_SHIFT);
        int ey = y + (this->y << CHUNK_SHIFT);
        Enemy* e = new_Enemy(ex, ey, cell->biome);
        enemies->addLast(enemies, e);
        return;
    }
    Node* node = hasEnemyInCell(this, x, y);
    if (node == NULL) return;
    Enemy* e = node->data;
    if (e->isBoss) return;
    enemies->removeNode(enemies, node);
    e->free(e);
    cell->type = CELL_GRAVE_INFESTED;
}

static void updateHeaven(Chunk* this, int x, int y) {
    Cell* cell = this->cellAt(this, x, y);
    if (cell->type == CELL_PORTAL) return;
    cell->biome = VIOLENCIA;
    cell->type = CELL_HEAVEN;

    LinkedList* enemies = this->enemies;
    while (enemies->head) {
        Enemy* e = enemies->removeFirst(enemies);
        free(e);
    }
}

static void update(Chunk* this, int biome, bool heaven) {
    int y = rand() & CHUNK_MASK;
    int x = rand() & CHUNK_MASK;
    updateDegenerescence(this, x, y, biome);
    updateFire(this, x, y);
    updateGraveInfested(this, x, y);
    if (heaven){
        for (int i = 0; i < HEAVEN_SPEED; i++){
            y = rand() & CHUNK_MASK;
            x = rand() & CHUNK_MASK;
            updateHeaven(this, x, y);
        } 
    }
        
}

static void _free(Chunk* this) {
    Node* cur = this->enemies->head;
    Node* temp;
    while (cur != NULL) {
        temp = cur;
        Enemy* e = cur->data;
        e->free(e);
        cur = cur->next;
        free(temp);
    }
    free(this->enemies);
    free(this);
}

Chunk* new_Chunk(int x, int y) {
    Chunk* this = malloc(sizeof(Chunk));
    this->x = x;
    this->y = y;
    this->enemies = new_LinkedList();

    this->randCounter = 0;

    this->update = update;
    this->resetDistance = resetDistance;
    this->cellAt = cellAt;
    this->free = _free;
    return this;
}