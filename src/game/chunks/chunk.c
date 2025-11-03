#include "chunk.h"

#include <stdlib.h>

#include "enemy.h"

static const int BASE_X[4] = {0 * BIOME_WIDTH_CHUNKS, 1 * BIOME_WIDTH_CHUNKS,
                              2 * BIOME_WIDTH_CHUNKS, 3 * BIOME_WIDTH_CHUNKS};

static const int BOSS_BASE[4][2] = {
    {HALF_WIDTH_CHUNKS, 1},
    {BIOME_WIDTH_CHUNKS + HALF_WIDTH_CHUNKS, 5},
    {BIOME_WIDTH_CHUNKS * 2 + HALF_WIDTH_CHUNKS, 7},
    {BIOME_WIDTH_CHUNKS * 3 + HALF_WIDTH_CHUNKS, 3}};

static const int TEMPLE_MATRIX[CELLS_PER_CHUNK] = {
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
};

static void resetDistance(Chunk* this) {
    Cell* cells = this->cells;
    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        cells[i].distance = -1;
    }
}

static void generateEnemies(Chunk* this) {
    int totalEnemies = rand() % (MAX_ENIMIES_PER_CHUNK + 1);
    static const int range = CHUNK_SIZE >> 1;
    LinkedList* enemies = this->enemies;
    for (int i = 0; i < totalEnemies; i++) {
        int cx = ((rand() % range) << 1) | 1;
        int cy = ((rand() % range) << 1) | 1;
        int x = cx + (this->x << CHUNK_SHIFT);
        int y = cy + (this->y << CHUNK_SHIFT);
        Enemy* e =
            new_Enemy(x, y, this->cells[(cy << CHUNK_SHIFT) | cx].biomeType);
        enemies->addLast(enemies, e);
    }
}

static void generateWalls(Chunk* this) {
    srand(this->x + this->y * SEED);
    for (int y = 0; y < CHUNK_SIZE; y++) {
        int yIdx = y << CHUNK_SHIFT;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            bool evenOrEven = ((y & 1) == 0 || (x & 1) == 0);
            Cell* cell = &this->cells[yIdx | x];
            cell->isBoss = 0;
            cell->isWall = evenOrEven;
            if ((y & 1) == 0 && (x & 1) == 0) continue;
            if (rand() % 100 <= 65) cell->isWall = 0;
        }
    }
}

static void generateBiome(Chunk* this) {
    int idx = this->x / BIOME_WIDTH_CHUNKS;

    if (idx > 3) idx = 3;

    bool isTransition = false;
    int x0, x1, delta;

    if (idx > 0 && BASE_X[idx] == this->x) {
        isTransition = true;
        x0 = ((this->x + this->y) * SEED) & CHUNK_MASK;
        x1 = ((this->x + this->y + 1) * SEED) & CHUNK_MASK;

        delta = x1 - x0;
    }

    int bx;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        if (isTransition) {
            float t = i / (float)CHUNK_SIZE;
            bx = x0 + delta * t;
        }
        int y = i << CHUNK_SHIFT;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int biome = idx + 1;
            if (isTransition) {
                biome--;
                if (x > bx) biome++;
                if (x - bx == 1 && x > 0 && x < 15) {
                    for (int k = -1; k < 2; k++)
                        this->cells[y | (x + k)].isWall = 0;
                }
            }
            this->cells[y | x].biomeType = biome;
        }
    }
}

static void generateStructures(Chunk* this) {
    int idx = this->x / BIOME_WIDTH_CHUNKS;
    bool isBossTemple =
        BOSS_BASE[idx][0] == this->x && BOSS_BASE[idx][1] == this->y;
    if (!isBossTemple) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int idx = y | x;
            Cell* cell = &this->cells[y | x];
            cell->isWall = TEMPLE_MATRIX[idx];
            cell->isBoss = 1;
        }
    }
}

static void generate(Chunk* this) {
    generateWalls(this);
    generateBiome(this);
    generateStructures(this);
    generateEnemies(this);
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

    generate(this);
    resetDistance(this);

    this->resetDistance = resetDistance;
    this->free = _free;
    return this;
}