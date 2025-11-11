#include "chunk_loader.h"
#include <stdint.h>
#include <stdlib.h>

static const int TEMPLE_MATRIX[CELLS_PER_CHUNK] = {
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
};

static const int FONT_MATRIX[25] = {
    1, 0, 0, 0, 1,
    0, 0, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 0, 0, 0,
    1, 0, 0, 0, 1
};

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

static inline unsigned int randChunk(ChunkLoader* this, Chunk* chunk) {
    static const uint64_t A = 0x9E3779B97F4A7C15ULL;
    uint64_t s = ((uint64_t)chunk->x << 32) ^ chunk->y ^ this->seed ^ chunk->randCounter++;
    s += A;
    s = (s ^ (s >> 30)) * 0xBF58476D1CE4E5B9ULL;
    s = (s ^ (s >> 27)) * 0x94D049BB133111EBULL;
    s ^= s >> 31;
    return (unsigned int)s;
}

static void preLoad(ChunkLoader* this, Chunk* chunk) {
    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        chunk->cells[i] = (Cell){ 0 };
        chunk->cells[i].distance = -1;
    }

    int biome = (chunk->x + 1) / this->biomeWidthChunks;
    chunk->biome = biome < 4 ? biome : 3;

    chunk->isBorder = chunk->y == 0 || chunk->y == (this->height - 1);

    chunk->type = CHUNK_NORMAL;

    if ((chunk->x + 1) % this->biomeWidthChunks == 0) {
        chunk->type = CHUNK_TRANSITION;
        return;
    }

    int biomeHalfWidth = (this->biomeWidthChunks >> 1) - 1;
    bool isXstructure = ((chunk->x - biomeHalfWidth) % this->biomeWidthChunks) == 0;
    int templeY = ((hash2D(chunk->x, 1, this->seed) & 5) + 1);

    if (isXstructure && templeY == chunk->y) {
        chunk->type = CHUNK_TEMPLE;
        return;
    }

    bool isYFont = ((templeY + 3) & 5) + 1 == chunk->y;
    if (isXstructure && isYFont) {
        chunk->type = CHUNK_FONT;
        return;
    }

    int fragmentX = (hash2D(chunk->biome, 1, this->seed) % (this->biomeWidthChunks - 2) + 1) + this->biomeWidthChunks * chunk->biome;
    int fragmentY = (hash2D(1, chunk->biome, this->seed) & 1) * (this->height - 1);

    if (chunk->x == fragmentX && chunk->y == fragmentY) {
        chunk->type = CHUNK_FRAGMENT;
        return;
    }
}

static void generateBorder(ChunkLoader* this, Chunk* chunk) {
    if (!chunk->isBorder) return;

    int mask = 7;

    int y0 = 3 + (hash2D(chunk->x, chunk->y, this->seed) & mask);
    int y1 = 3 + (hash2D(chunk->x + 1, chunk->y, this->seed) & mask);

    int delta = y1 - y0;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int yLevel = y0 + delta * (i / (float)CHUNK_SIZE);
        if (chunk->y == 0)
            for (int j = yLevel; j >= 0; j--) {
                chunk->cellAt(chunk, i, j)->type = CELL_WALL;
            } else
                for (int j = yLevel; j < CHUNK_SIZE; j++) {
                    chunk->cellAt(chunk, i, j)->type = CELL_WALL;
                }
    }
}

static void generateBiomeTransition(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type != CHUNK_TRANSITION) return;

    int x0 = hash2D(chunk->x, chunk->y, this->seed) & CHUNK_MASK;
    int x1 = hash2D(chunk->x, chunk->y + 1, this->seed) & CHUNK_MASK;

    int delta = x1 - x0;

    for (int y = 0; y < CHUNK_SIZE; y++) {
        float t = y / (float)CHUNK_SIZE;
        int bx = x0 + delta * t;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int biome = chunk->biome - 1;
            if (x > bx) biome++;
            chunk->cellAt(chunk, x, y)->biome = biome;
        }
    }

    for (int k = 0; k < 9; k++) {
        int stoneX = 1 + randChunk(this, chunk) % (CHUNK_SIZE - 2);
        int stoneY = 1 + randChunk(this, chunk) % (CHUNK_SIZE - 2);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                chunk->cellAt(chunk, j + stoneX, i + stoneY)->type = CELL_WALL;
            }
        }
    }
}

static void generateBiome(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type == CHUNK_TRANSITION) return;
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            chunk->cellAt(chunk, x, y)->biome = chunk->biome;
        }
    }
}

static void generateTemple(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type != CHUNK_TEMPLE) return;

    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        int templeCell = TEMPLE_MATRIX[i];
        chunk->cells[i].type = templeCell == 0 ? CELL_TEMPLE : CELL_WALL;
    }
}

static void generateFont(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type != CHUNK_FONT) return;
    int startX = hash2D(chunk->x, chunk->y, this->seed) % 11;
    int startY = hash2D(chunk->y, chunk->x, this->seed) % 11;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            Cell* cell = chunk->cellAt(chunk, j + startX, i + startY);
            int fontCell = FONT_MATRIX[i * 5 + j];
            cell->type = fontCell == 0 ? CELL_FONT_HEALTH : CELL_WALL;
        }
    }
}

static void generateWalls1x1(ChunkLoader* this, Chunk* chunk) {
    if (isStructure(chunk->type) || chunk->biome != 0) return;
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            Cell* cell = chunk->cellAt(chunk, x, y);
            if (((y & 1) == 0 || (x & 1) == 0))
                cell->type = CELL_WALL;
            if ((y & 1) == 0 && (x & 1) == 0) continue;
            if (randChunk(this, chunk) % 100 < WALL_DENSITY) {
                cell->type = CELL_EMPTY;
            }
        }
    }
}

static void generateWalls2x2(ChunkLoader* this, Chunk* chunk) {
    if (isStructure(chunk->type) || (chunk->biome != 1 && chunk->biome != 3) ||
        chunk->type == CHUNK_TRANSITION)
        return;

    int startX = -(chunk->x % 3);
    int startY = -(chunk->y % 3);

    for (int i = startY; i < CHUNK_SIZE; i += 3) {
        if (i < 0 || i >= CHUNK_SIZE) continue;  // evita índices negativos
        for (int j = startX; j < CHUNK_SIZE; j += 3) {
            if (j < 0 || j >= CHUNK_SIZE) continue;

            for (int k = 0; k < 3; k++) {
                if (j + k < CHUNK_SIZE && j + k >= 0)
                    chunk->cellAt(chunk, j + k, i)->type = CELL_WALL;

                if (j - k >= 0 && j - k < CHUNK_SIZE)
                    chunk->cellAt(chunk, j - k, i)->type = CELL_WALL;

                if (i + k < CHUNK_SIZE && i + k >= 0)
                    chunk->cellAt(chunk, j, i + k)->type = CELL_WALL;

                if (i - k >= 0 && i - k < CHUNK_SIZE)
                    chunk->cellAt(chunk, j, i - k)->type = CELL_WALL;
            }
        }
    }

    for (int i = startY; i < CHUNK_SIZE; i += 3) {
        if (i < 0 || i >= CHUNK_SIZE) continue;
        for (int j = startX; j < CHUNK_SIZE; j += 3) {
            if (j < 0 || j >= CHUNK_SIZE) continue;

            if ((randChunk(this, chunk) % 100 < WALL_DENSITY)) {
                for (int k = 0; k < 2; k++) {
                    int idx = i + k + 1;
                    if (idx >= 0 && idx < CHUNK_SIZE)
                        chunk->cellAt(chunk, j, idx)->type = CELL_EMPTY;
                }
            }

            if (randChunk(this, chunk) % 100 < WALL_DENSITY) {
                for (int k = 0; k < 2; k++) {
                    int idx = j + k + 1;
                    if (idx >= 0 && idx < CHUNK_SIZE)
                        chunk->cellAt(chunk, idx, i)->type = CELL_EMPTY;
                }
            }
        }
    }
}

static void generateWalls3x3(ChunkLoader* this, Chunk* chunk) {
    if (isStructure(chunk->type) || chunk->biome != 2 || chunk->type == CHUNK_TRANSITION)
        return;
    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            for (int k = 0; k < 4; k++) {
                chunk->cellAt(chunk, j + k, i)->type = CELL_WALL;
                chunk->cellAt(chunk, i, j + k)->type = CELL_WALL;
            }
        }
    }

    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            if (randChunk(this, chunk) % 100 < WALL_DENSITY) {
                for (int k = 0; k < 3; k++) {
                    chunk->cellAt(chunk, j + k + 1, i)->type = CELL_EMPTY;
                }
            }
            if (randChunk(this, chunk) % 100 < WALL_DENSITY) {
                for (int k = 0; k < 3; k++) {
                    chunk->cellAt(chunk, j, i + k + 1)->type = CELL_EMPTY;
                }
            }
        }
    }
}

static void generateWind(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type == CHUNK_TRANSITION || isStructure(chunk->type) || chunk->biome != 0 ||
        chunk->isBorder)
        return;

    bool horizontal = randChunk(this, chunk) & 1;
    int windIdx = randChunk(this, chunk) & CHUNK_MASK;
    int windDir;
    if (horizontal)
        windDir = (randChunk(this, chunk) & 1) + CELL_WIND_RIGHT;
    else
        windDir = (randChunk(this, chunk) & 1) + CELL_WIND_UP;
    if (horizontal) {
        for (int i = 0; i < CHUNK_SIZE; i++) {
            Cell* cell = chunk->cellAt(chunk, i, windIdx);
            cell->type = windDir;
        }
    } else {
        for (int i = 0; i < CHUNK_SIZE; i++) {
            Cell* cell = chunk->cellAt(chunk, windIdx, i);
            cell->type = windDir;
        }
    }
}

static void generateMud(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type == CHUNK_TRANSITION || isStructure(chunk->type) || chunk->biome != 1) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            Cell* cell = chunk->cellAt(chunk, j, i);
            if (cell->type == CELL_WALL) continue;
            if (randChunk(this, chunk) % 100 < MUD_DENSITY) cell->type = CELL_MUD;
        }
    }
}

static void generateGraves(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type == CHUNK_TRANSITION || isStructure(chunk->type) || chunk->biome != 2) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            Cell* cell = chunk->cellAt(chunk, j, i);
            if (cell->type == CELL_WALL) continue;
            if (randChunk(this, chunk) % 100 < GRAVE_DENSITY) {
                if (randChunk(this, chunk) % 100 < GRAVE_INFESTED_PROBABILITY)
                    cell->type = CELL_GRAVE_INFESTED;
                else
                    cell->type = CELL_GRAVE;
            }
        }
    }
}

static void generateFire(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type == CHUNK_TRANSITION || isStructure(chunk->type) || chunk->biome != 2) return;
    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            if (randChunk(this, chunk) % 100 < FIRE_DENSITY &&
                chunk->cellAt(chunk, j + 1, i)->type == CELL_EMPTY) {
                for (int k = 0; k < 2; k++) {
                    chunk->cellAt(chunk, j + 1 + k, i)->type = CELL_FIRE_ON;
                }
            }
            if (randChunk(this, chunk) % 100 < FIRE_DENSITY &&
                chunk->cellAt(chunk, j, i + 1)->type == CELL_EMPTY) {
                for (int k = 0; k < 2; k++) {
                    chunk->cellAt(chunk, j, i + 1 + k)->type = CELL_FIRE_ON;
                }
            }
        }
    }
}

static void generateSpikes(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type == CHUNK_TRANSITION || isStructure(chunk->type) || chunk->biome != 3) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            Cell* cell = chunk->cellAt(chunk, j, i);
            if (cell->type == CELL_WALL) continue;
            if (randChunk(this, chunk) % 100 < SPIKE_DENSITY) cell->type = CELL_SPIKE;
        }
    }
}

static void generateCoins(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type == CHUNK_TEMPLE) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        for (int j = 0; j < CHUNK_SIZE; j++) {
            Cell* cell = chunk->cellAt(chunk, j, i);
            if (cell->type != CELL_EMPTY) continue;
            if (randChunk(this, chunk) % 100 < COIN_DENSITY) cell->type = CELL_COIN;
        }
    }
}

static void generateFragement(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type != CHUNK_FRAGMENT) return;
    
    Cell* cell;
    int idx = randChunk(this, chunk) & TOTAL_CELLS_MASK;
    for (int i = 0; i < CELLS_PER_CHUNK; i++){
        cell = &chunk->cells[idx];
        if (cell->type == CELL_EMPTY) break;
        idx = (idx + 1) & TOTAL_CELLS_MASK;
    }

    cell->type = CELL_FRAGMENT;
}

static void generateFruit(ChunkLoader* this, Chunk* chunk) {
    if (chunk->type == CHUNK_TRANSITION || randChunk(this, chunk) % 100 > FRUIT_PROBABILITY) return;

    Cell* cell;
    int idx = randChunk(this, chunk) & TOTAL_CELLS_MASK;
    for (int i = 0; i < CELLS_PER_CHUNK; i++){
        cell = &chunk->cells[idx];
        if (cell->type == CELL_EMPTY) break;
        idx = (idx + 1) & TOTAL_CELLS_MASK;
    }
    cell->type = CELL_FRUIT;
}

static void generateEnemies(ChunkLoader* this, Chunk* chunk) {
    if (isStructure(chunk->type) || chunk->isBorder) return;
    if (randChunk(this, chunk) % 100 >= BIOME_ENEMY_PROBABILITY[chunk->biome]) return;
    LinkedList* enemies = chunk->enemies;
    Cell* cell;
    int idx = randChunk(this, chunk) & TOTAL_CELLS_MASK;
    for (int i = 0; i < CELLS_PER_CHUNK; i++){
        cell = &chunk->cells[idx];
        if (isPassable(cell->type)) break;
        idx = (idx + 1) & TOTAL_CELLS_MASK;
    }

    int ex = (idx & CHUNK_MASK) + (chunk->x << CHUNK_SHIFT);
    int ey = (idx >> CHUNK_SHIFT) + (chunk->y << CHUNK_SHIFT);
    
    Enemy* e = new_Enemy(ex, ey, cell->biome);
    enemies->addLast(enemies, e);
}

typedef void (*ChunkGeneratorFn)(ChunkLoader*, Chunk*);

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
    generateFragement,
    generateFruit,
    generateEnemies,
    generateCoins,
};

void generate(ChunkLoader* this, Chunk* chunk) {
    preLoad(this, chunk);
    for (int i = 0; i < sizeof(GENERATORS) / sizeof(GENERATORS[0]); i++) {
        GENERATORS[i](this, chunk);
    }
}

static void _free(ChunkLoader* this) {
    free(this);
}

ChunkLoader* new_ChunkLoader(const int width, const int height, const int seed) {
    ChunkLoader* this = malloc(sizeof(ChunkLoader));
    this->width = width;
    this->height = height;
    this->seed = seed;
    this->biomeWidthChunks = width >> 2;

    this->generate = generate;
    this->free = _free;
    return this;
}