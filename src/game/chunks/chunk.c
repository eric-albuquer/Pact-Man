#include "chunk.h"

#include <stdint.h>
#include <stdlib.h>

#include "enemy.h"

typedef struct {
    int width, height;
    int biomeWidthChunks;
    int maxEnemiesPerChunk;
    int seed;
} ChunkConfig;

static ChunkConfig config = {20, 9, 5, 2, 51616723};

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

static const int FONT_MATRIX[25] = {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1};

static inline unsigned int hash2D(int x, int y, unsigned int seed) {
    uint64_t h = (uint64_t)(x) * 0x9E3779B185EBCA87ULL;
    h ^= (uint64_t)(y) * 0xC2B2AE3D27D4EB4FULL;
    h ^= (uint64_t)seed * 0x165667B19E3779F9ULL;
    h ^= (h >> 33);
    h *= 0xFF51AFD7ED558CCDULL;
    h ^= (h >> 33);
    h *= 0xC4CEB9FE1A85EC53ULL;
    h ^= (h >> 33);
    return (unsigned int)h;
}

static inline unsigned int randChunk(Chunk* this) {
    static const uint64_t A = 0x9E3779B97F4A7C15ULL;
    uint64_t s =
        ((uint64_t)this->x << 32) ^ this->y ^ config.seed ^ this->randCounter++;
    s += A;
    s = (s ^ (s >> 30)) * 0xBF58476D1CE4E5B9ULL;
    s = (s ^ (s >> 27)) * 0x94D049BB133111EBULL;
    s ^= s >> 31;
    return (unsigned int)s;
}

static inline Cell* cellAt(Chunk* c, int x, int y) {
    return &c->cells[(y << CHUNK_SHIFT) | x];
}

void setArgs(int width, int height, int maxEnemiesPerChunk, int seed) {
    config.width = width;
    config.height = height;
    config.biomeWidthChunks = (width >> 2) | 1;
    config.maxEnemiesPerChunk = maxEnemiesPerChunk;
    config.seed = seed;
}

static void resetDistance(Chunk* this) {
    Cell* cells = this->cells;
    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        cells[i].distance = -1;
    }
}

static void preLoad(Chunk* this) {
    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        this->cells[i] = (Cell){0};
        this->cells[i].distance = -1;
    }

    this->isTransition = this->x % config.biomeWidthChunks == 0 && this->x > 0;
    int biome = this->x / config.biomeWidthChunks;
    this->biome = biome < 4 ? biome : 3;

    this->isBorder = this->y == 0 || this->y == (config.height - 1);

    int biomeHalfWidth = config.biomeWidthChunks >> 1;
    bool isXstructure =
        ((this->x - biomeHalfWidth) % config.biomeWidthChunks) == 0;
    int templeY = ((hash2D(this->x, 1, config.seed) & 5) + 1);
    bool isYTemple = templeY == this->y;
    this->isTemple = isXstructure && isYTemple;

    bool isYFont = ((templeY + 3) & 5) + 1 == this->y;
    this->isFont = isXstructure && isYFont;
    this->isStructure = this->isFont || this->isTemple;

    int fragmentX =
        (hash2D(this->biome, 1, config.seed) % (config.biomeWidthChunks - 2) +
         1) +
        config.biomeWidthChunks * this->biome;
    int fragmentY =
        (hash2D(1, this->biome, config.seed) & 1) * (config.height - 1);

    this->fragment = this->x == fragmentX && this->y == fragmentY;
}

static void generateBorder(Chunk* this) {
    if (!this->isBorder) return;

    int mask = 7;

    int y0 = 3 + (hash2D(this->x, this->y, config.seed) & mask);
    int y1 = 3 + (hash2D(this->x + 1, this->y, config.seed) & mask);

    int delta = y1 - y0;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int yLevel = y0 + delta * (i / (float)CHUNK_SIZE);
        if (this->y == 0)
            for (int j = yLevel; j >= 0; j--) {
                cellAt(this, i, j)->isWall = 1;
            }
        else
            for (int j = yLevel; j < CHUNK_SIZE; j++) {
                cellAt(this, i, j)->isWall = 1;
            }
    }
}

static void generateBiomeTransition(Chunk* this) {
    if (!this->isTransition) return;
    int idx = this->x / config.biomeWidthChunks;

    if (idx > 3) idx = 3;

    int x0 = hash2D(this->x, this->y, config.seed) & CHUNK_MASK;
    int x1 = hash2D(this->x, this->y + 1, config.seed) & CHUNK_MASK;

    int delta = x1 - x0;

    for (int y = 0; y < CHUNK_SIZE; y++) {
        float t = y / (float)CHUNK_SIZE;
        int bx = x0 + delta * t;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int biome = idx - 1;
            if (x > bx) biome++;
            cellAt(this, x, y)->biomeType = biome;
        }
    }

    for (int k = 0; k < 6; k++) {
        int stoneX = 1 + (randChunk(this) % (CHUNK_SIZE - 4));
        int stoneY = 1 + (randChunk(this) % (CHUNK_SIZE - 4));

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                cellAt(this, j + stoneX, i + stoneY)->isWall = 1;
            }
        }
    }
}

static void generateBiome(Chunk* this) {
    if (this->isTransition) return;
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            cellAt(this, x, y)->biomeType = this->biome;
        }
    }
}

static void generateTemple(Chunk* this) {
    if (!this->isTemple) return;

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            Cell* cell = cellAt(this, x, y);
            cell->isWall = TEMPLE_MATRIX[(y << CHUNK_SHIFT) | x];
            cell->isBossTemple = 1;
        }
    }
}

static void generateFont(Chunk* this) {
    if (!this->isFont) return;
    int startX = hash2D(this->x, this->y, config.seed) % 10;
    int startY = hash2D(this->y, this->x, config.seed) % 10;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            Cell* cell = cellAt(this, j + startX, i + startY);
            cell->isWall = FONT_MATRIX[i * 5 + j];
            cell->isFont = 1;
        }
    }
}

static void generateWalls1x1(Chunk* this) {
    if (this->isStructure || this->biome != 0) return;
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            Cell* cell = cellAt(this, x, y);
            bool evenOrEven = ((y & 1) == 0 || (x & 1) == 0);
            cell->isWall = evenOrEven;
            if ((y & 1) == 0 && (x & 1) == 0) continue;
            if (randChunk(this) % 100 <= 65) {
                cell->isWall = 0;
            }
        }
    }
}

static void generateWalls2x2(Chunk* this) {
    if (this->isStructure || (this->biome != 1 && this->biome != 3) ||
        this->isTransition)
        return;

    int startX = -(this->x % 3);
    int startY = -(this->y % 3);

    for (int i = startY; i < CHUNK_SIZE; i += 3) {
        if (i < 0 || i >= CHUNK_SIZE) continue;  // evita índices negativos
        for (int j = startX; j < CHUNK_SIZE; j += 3) {
            if (j < 0 || j >= CHUNK_SIZE) continue;

            for (int k = 0; k < 3; k++) {
                if (j + k < CHUNK_SIZE && j + k >= 0)
                    cellAt(this, j + k, i)->isWall = 1;

                if (j - k >= 0 && j - k < CHUNK_SIZE)
                    cellAt(this, j - k, i)->isWall = 1;

                if (i + k < CHUNK_SIZE && i + k >= 0)
                    cellAt(this, j, i + k)->isWall = 1;

                if (i - k >= 0 && i - k < CHUNK_SIZE)
                    cellAt(this, j, i - k)->isWall = 1;
            }
        }
    }

    for (int i = startY; i < CHUNK_SIZE; i += 3) {
        if (i < 0 || i >= CHUNK_SIZE) continue;
        for (int j = startX; j < CHUNK_SIZE; j += 3) {
            if (j < 0 || j >= CHUNK_SIZE) continue;

            if ((randChunk(this) % 100 < 70)) {
                for (int k = 0; k < 2; k++) {
                    int idx = i + k + 1;
                    if (idx >= 0 && idx < CHUNK_SIZE)
                        cellAt(this, j, idx)->isWall = 0;
                }
            }

            if (randChunk(this) % 100 < 70) {
                for (int k = 0; k < 2; k++) {
                    int idx = j + k + 1;
                    if (idx >= 0 && idx < CHUNK_SIZE)
                        cellAt(this, idx, i)->isWall = 0;
                }
            }
        }
    }
}

static void generateWalls3x3(Chunk* this) {
    if (this->isStructure || this->biome != 2 || this->isTransition) return;
    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            for (int k = 0; k < 4; k++) {
                cellAt(this, j + k, i)->isWall = 1;
                cellAt(this, i, j + k)->isWall = 1;
            }
        }
    }

    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            if (randChunk(this) % 100 < 70) {
                for (int k = 0; k < 3; k++) {
                    cellAt(this, j + k + 1, i)->isWall = 0;
                }
            }
            if (randChunk(this) % 100 < 70) {
                for (int k = 0; k < 3; k++) {
                    cellAt(this, j, i + k + 1)->isWall = 0;
                }
            }
        }
    }
}

static void generateWind(Chunk* this) {
    if (this->isTransition || this->isStructure || this->biome > 0 ||
        this->isBorder)
        return;

    bool horizontal = randChunk(this) & 1;
    int windIdx = randChunk(this) & CHUNK_MASK;
    int windDir;
    if (horizontal)
        windDir = (randChunk(this) & 1) + 1;
    else
        windDir = (randChunk(this) & 1) + 3;
    if (horizontal) {
        for (int i = 0; i < CHUNK_SIZE; i++) {
            Cell* cell = cellAt(this, i, windIdx);
            cell->windDir = windDir;
            cell->isWall = 0;
        }
    } else {
        for (int i = 0; i < CHUNK_SIZE; i++) {
            Cell* cell = cellAt(this, windIdx, i);
            cell->windDir = windDir;
            cell->isWall = 0;
        }
    }
}

static void generateMud(Chunk* this) {
    if (this->isTransition || this->isStructure || this->biome != 1) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            if (this->cells[y | j].isWall) continue;
            if (randChunk(this) % 100 < 10) this->cells[y | j].mud = 1;
        }
    }
}

static void generateGraves(Chunk* this) {
    if (this->isTransition || this->isStructure || this->biome != 2) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            if (this->cells[y | j].isWall) continue;
            if (randChunk(this) % 100 < 5) this->cells[y | j].grave = 1;
        }
    }
}

static void generateFire(Chunk* this) {
    if (this->isTransition || this->isStructure || this->biome != 2) return;
    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            if (randChunk(this) % 100 < 40 &&
                this->cells[y | (j + 1)].isWall == 0) {
                for (int k = 0; k < 2; k++) {
                    this->cells[y | (j + k + 1)].fire = 1;
                }
            }
            if (randChunk(this) % 100 < 40 &&
                this->cells[((i + 1) << CHUNK_SHIFT) | j].isWall == 0) {
                for (int k = 0; k < 2; k++) {
                    this->cells[((i + k + 1) << CHUNK_SHIFT) | j].fire = 1;
                }
            }
        }
    }
}

static void generateSpikes(Chunk* this) {
    if (this->isTransition || this->isStructure || this->biome != 3) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            if (this->cells[y | j].isWall) continue;
            if (randChunk(this) % 100 < 5) this->cells[y | j].spike = 1;
        }
    }
}

static void generateCoins(Chunk* this) {
    if (this->isStructure) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            if (this->cells[y | j].isWall) continue;
            if (randChunk(this) & 1) this->cells[y | j].coin = 1;
        }
    }
}

static void generateFragement(Chunk* this) {
    if (!this->fragment) return;
    int x, y;
    do {
        x = randChunk(this) & CHUNK_MASK;
        y = randChunk(this) & CHUNK_MASK;
    } while (this->cells[(y << CHUNK_SHIFT) | x].isWall);

    this->cells[(y << CHUNK_SHIFT) | x].fragment = 1;
}

static void generateEnemies(Chunk* this) {
    if (this->isStructure || this->isBorder) return;
    int totalEnemies = randChunk(this) % (config.maxEnemiesPerChunk + 1);
    static const int range = CHUNK_SIZE >> 1;
    LinkedList* enemies = this->enemies;
    for (int i = 0; i < totalEnemies; i++) {
        int cx, cy, x, y;
        do {
            cx = ((randChunk(this) % range) << 1) | 1;
            cy = ((randChunk(this) % range) << 1) | 1;
            x = cx + (this->x << CHUNK_SHIFT);
            y = cy + (this->y << CHUNK_SHIFT);
        } while (this->cells[(cy << CHUNK_SHIFT) | cx].isWall);
        Enemy* e =
            new_Enemy(x, y, this->cells[(cy << CHUNK_SHIFT) | cx].biomeType);
        enemies->addLast(enemies, e);
    }
}

typedef void (*ChunkGeneratorFn)(Chunk*);

static ChunkGeneratorFn GENERATORS[] = {
    generateBiomeTransition,
    generateBiome,
    generateTemple,
    generateFont,
    generateWalls1x1,
    generateWalls2x2,
    generateWalls3x3,
    generateBorder,
    generateWind,
    generateMud,
    generateGraves,
    generateFire,
    generateSpikes,
    generateCoins,
    generateFragement,
    generateEnemies,
};

static void generate(Chunk* this) {
    preLoad(this);
    for (int i = 0; i < sizeof(GENERATORS) / sizeof(GENERATORS[0]); i++) {
        GENERATORS[i](this);
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

    generate(this);

    this->resetDistance = resetDistance;
    this->free = _free;
    return this;
}