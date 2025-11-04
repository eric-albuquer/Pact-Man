#include "chunk.h"

#include <stdlib.h>

#include "enemy.h"

static int WIDTH = 20;
static int HEIGHT = 9;
static int BIOME_WIDTH_CHUNKS = 5;
static int BIOME_HALF_WIDTH = 2;
static int MAX_ENEMIES_PER_CHUNK = 2;
static int SEED = 51616723;

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

void setArgs(const int width, const int height, const int maxEnemiesPerChunk,
             const int seed) {
    int bw = (width >> 2) | 1;
    WIDTH = (bw + 1) << 2;
    HEIGHT = height;
    BIOME_WIDTH_CHUNKS = bw;
    BIOME_HALF_WIDTH = bw >> 1;
    MAX_ENEMIES_PER_CHUNK = maxEnemiesPerChunk;
    SEED = seed;
}

static void resetDistance(Chunk* this) {
    Cell* cells = this->cells;
    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        cells[i].distance = -1;
    }
}

static void initCells(Chunk* this) {
    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        this->cells[i] = (Cell){0, 0, 0, -1, 0, 0};
    }

    this->isTransition = this->x % BIOME_WIDTH_CHUNKS == 0 && this->x > 0;
    int biome = this->x / BIOME_WIDTH_CHUNKS;
    this->biome = biome < 4 ? biome : 3;
    bool isXTemple = ((this->x - BIOME_HALF_WIDTH) % BIOME_WIDTH_CHUNKS) == 0;
    bool isYTemple = (((SEED * this->x) & 7) + 1) == this->y;
    this->isTemple = isXTemple && isYTemple;
    this->isBorder = this->y == 0 || this->y == (HEIGHT - 1);
}

static void generateBorder(Chunk* this) {
    if (!this->isBorder) return;

    int mask = 7;

    int y0 = 3 + (((this->x + this->y) * SEED) & mask);
    int y1 = 3 + (((this->x + 1 + this->y) * SEED) & mask);

    int delta = y1 - y0;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int yLevel = y0 + delta * (i / (float)CHUNK_SIZE);
        if (this->y == 0)
            for (int j = yLevel; j >= 0; j--) {
                this->cells[(j << CHUNK_SHIFT) | i].isWall = 1;
            }
        else
            for (int j = yLevel; j < CHUNK_SIZE; j++) {
                this->cells[(j << CHUNK_SHIFT) | i].isWall = 1;
            }
    }
}

static void generateBiomeTransition(Chunk* this) {
    if (!this->isTransition) return;
    int idx = this->x / BIOME_WIDTH_CHUNKS;

    if (idx > 3) idx = 3;

    int x0 = ((this->x + this->y) * SEED) & CHUNK_MASK;
    int x1 = ((this->x + this->y + 1) * SEED) & CHUNK_MASK;

    int delta = x1 - x0;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        float t = i / (float)CHUNK_SIZE;
        int bx = x0 + delta * t;
        int y = i << CHUNK_SHIFT;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int biome = idx - 1;
            if (x > bx) biome++;
            if (x - bx == 1 && x > 0 && x < 15) {
                for (int k = -1; k < 2; k++)
                    this->cells[y | (x + k)].isWall = 2;
            }
            this->cells[y | x].biomeType = biome;
        }
    }
}

static void generateBiome(Chunk* this) {
    if (this->isTransition) return;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            this->cells[y | x].biomeType = this->biome;
        }
    }
}

static void generateStructures(Chunk* this) {
    if (!this->isTemple) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int idx = y | x;
            Cell* cell = &this->cells[y | x];
            cell->isWall = TEMPLE_MATRIX[idx];
            cell->isBossTemple = 1;
        }
    }
}

static void generateWalls(Chunk* this) {
    if (this->isTemple) return;
    for (int y = 0; y < CHUNK_SIZE; y++) {
        int yIdx = y << CHUNK_SHIFT;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            Cell* cell = &this->cells[yIdx | x];
            if (cell->isWall == 2) {
                cell->isWall = 0;
                continue;
            }
            bool evenOrEven = ((y & 1) == 0 || (x & 1) == 0);
            cell->isWall = evenOrEven;
            if ((y & 1) == 0 && (x & 1) == 0) continue;
            if (rand() % 100 <= 65) {
                cell->isWall = 0;
                if (rand() & 1 && !this->isBorder) cell->coin = 1;
            }
        }
    }
}

static void generateWind(Chunk* this) {
    if (this->isTransition || this->isTemple || this->biome > 0 ||
        this->isBorder)
        return;

    bool horizontal = rand() & 1;
    int windIdx = rand() & CHUNK_MASK;
    int windDir;
    if (horizontal)
        windDir = (rand() & 1) + 1;
    else
        windDir = (rand() & 1) + 3;
    if (horizontal) {
        int y = windIdx << CHUNK_SHIFT;
        for (int i = 0; i < CHUNK_SIZE; i++) {
            Cell* cell = &this->cells[y | i];
            cell->windDir = windDir;
            cell->isWall = 0;
        }
    } else {
        for (int i = 0; i < CHUNK_SIZE; i++) {
            Cell* cell = &this->cells[(i << CHUNK_SHIFT) | windIdx];
            cell->windDir = windDir;
            cell->isWall = 0;
        }
    }
}

static void generateEnemies(Chunk* this) {
    if (this->isTemple || this->isBorder) return;
    int totalEnemies = rand() % (MAX_ENEMIES_PER_CHUNK + 1);
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

static void generate(Chunk* this) {
    srand(SEED * this->y + this->x);
    initCells(this);
    generateBiomeTransition(this);
    generateBiome(this);
    generateStructures(this);
    generateWalls(this);
    generateBorder(this);
    generateWind(this);
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

    this->resetDistance = resetDistance;
    this->free = _free;
    return this;
}