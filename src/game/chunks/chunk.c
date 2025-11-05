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

static const int FONT_MATRIX[25] = {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                    0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1};

static inline unsigned int hash2D_seed(int x, int y, unsigned int seed) {
    unsigned int h = (unsigned int)(x * 374761393 + y * 668265263 +
                                    seed * 1442695040888963407ULL);
    h = (h ^ (h >> 13)) * 1274126177;
    h = h ^ (h >> 16);
    return h;
}

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

static void preLoad(Chunk* this) {
    for (int i = 0; i < CELLS_PER_CHUNK; i++) {
        this->cells[i] = (Cell){0};
        this->cells[i].distance = -1;
    }

    this->isTransition = this->x % BIOME_WIDTH_CHUNKS == 0 && this->x > 0;
    int biome = this->x / BIOME_WIDTH_CHUNKS;
    this->biome = biome < 4 ? biome : 3;

    this->isBorder = this->y == 0 || this->y == (HEIGHT - 1);

    bool isXstructure =
        ((this->x - BIOME_HALF_WIDTH) % BIOME_WIDTH_CHUNKS) == 0;
    int templeY = ((hash2D_seed(this->x, 1, SEED) & 5) + 1);
    bool isYTemple = templeY == this->y;
    this->isTemple = isXstructure && isYTemple;

    bool isYFont = ((templeY + 3) & 5) + 1 == this->y;
    this->isFont = isXstructure && isYFont;
    this->isStructure = this->isFont || this->isTemple;

    int fragmentX = (hash2D_seed(this->biome, 1, SEED) % (BIOME_WIDTH_CHUNKS - 2) + 1) + BIOME_WIDTH_CHUNKS * this->biome;
    int fragmentY = (hash2D_seed(1, this->biome, SEED) & 1) * (HEIGHT - 1);

    this->fragment = this->x == fragmentX && this->y == fragmentY;
}

static void generateBorder(Chunk* this) {
    if (!this->isBorder) return;

    int mask = 7;

    int y0 = 3 + (hash2D_seed(this->x, this->y, SEED) & mask);
    int y1 = 3 + (hash2D_seed(this->x + 1, this->y, SEED) & mask);

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

    int x0 = hash2D_seed(this->x, this->y, SEED) & CHUNK_MASK;
    int x1 = hash2D_seed(this->x, this->y + 1, SEED) & CHUNK_MASK;

    int delta = x1 - x0;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        float t = i / (float)CHUNK_SIZE;
        int bx = x0 + delta * t;
        int y = i << CHUNK_SHIFT;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int biome = idx - 1;
            if (x > bx) biome++;
            this->cells[y | x].biomeType = biome;
        }
    }

    for (int k = 0; k < 6; k++) {
        int stoneX = 1 + (rand() % (CHUNK_SIZE - 4));
        int stoneY = 1 + (rand() & (CHUNK_SIZE - 4));

        for (int i = 0; i < 2; i++) {
            int y = (i + stoneY) << CHUNK_SHIFT;
            for (int j = 0; j < 2; j++) {
                this->cells[y | (j + stoneX)].isWall = 1;
            }
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

static void generateTemple(Chunk* this) {
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

static void generateFont(Chunk* this) {
    if (!this->isFont) return;
    int startX = hash2D_seed(this->x, this->y, SEED) % 10;
    int startY = hash2D_seed(this->y, this->x, SEED) % 10;

    for (int i = 0; i < 5; i++) {
        int y = (i + startY) << CHUNK_SHIFT;
        for (int j = 0; j < 5; j++) {
            int x = j + startX;
            this->cells[y | x].isWall = FONT_MATRIX[i * 5 + j];
            this->cells[y | x].isFont = 1;
        }
    }
}

static void generateWalls1x1(Chunk* this) {
    if (this->isStructure || this->biome != 0) return;
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
                    this->cells[i * CHUNK_SIZE + (j + k)].isWall = 1;

                if (j - k >= 0 && j - k < CHUNK_SIZE)
                    this->cells[i * CHUNK_SIZE + (j - k)].isWall = 1;

                if (i + k < CHUNK_SIZE && i + k >= 0)
                    this->cells[(i + k) * CHUNK_SIZE + j].isWall = 1;

                if (i - k >= 0 && i - k < CHUNK_SIZE)
                    this->cells[(i - k) * CHUNK_SIZE + j].isWall = 1;
            }
        }
    }

    for (int i = startY; i < CHUNK_SIZE; i += 3) {
        if (i < 0 || i >= CHUNK_SIZE) continue;
        for (int j = startX; j < CHUNK_SIZE; j += 3) {
            if (j < 0 || j >= CHUNK_SIZE) continue;

            if ((rand() % 100 < 70)) {
                for (int k = 0; k < 2; k++) {
                    int idx = i + k + 1;
                    if (idx >= 0 && idx < CHUNK_SIZE)
                        this->cells[(idx << CHUNK_SHIFT) | j].isWall = 0;
                }
            }

            if ((rand() % 100 < 70)) {
                for (int k = 0; k < 2; k++) {
                    int idx = j + k + 1;
                    if (idx >= 0 && idx < CHUNK_SIZE)
                        this->cells[(i << CHUNK_SHIFT) | idx].isWall = 0;
                }
            }
        }
    }
}

static void generateWalls3x3(Chunk* this) {
    if (this->isStructure || this->biome != 2 || this->isTransition) return;
    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            for (int k = 0; k < 4; k++) {
                this->cells[y | (j + k)].isWall = 1;
                this->cells[((j + k) << CHUNK_SHIFT) | i].isWall = 1;
            }
        }
    }

    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            if (rand() % 100 < 60) {
                for (int k = 0; k < 3; k++) {
                    this->cells[y | (j + k + 1)].isWall = 0;
                }
            }
            if (rand() % 100 < 60) {
                for (int k = 0; k < 3; k++) {
                    this->cells[((i + k + 1) << CHUNK_SHIFT) | j].isWall = 0;
                }
            }
        }
    }
}

static void generateWind(Chunk* this) {
    if (this->isTransition || this->isStructure || this->biome > 0 ||
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

static void generateMud(Chunk* this) {
    if (this->isTransition || this->isStructure || this->biome != 1) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            if (this->cells[y | j].isWall) continue;
            if (rand() % 100 < 10) this->cells[y | j].mud = 1;
        }
    }
}

static void generateGraves(Chunk* this) {
    if (this->isTransition || this->isStructure || this->biome != 2) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            if (this->cells[y | j].isWall) continue;
            if (rand() % 100 < 5) this->cells[y | j].grave = 1;
        }
    }
}

static void generateFire(Chunk* this){
    if (this->isTransition || this->isStructure || this->biome != 2) return;
    for (int i = 0; i < CHUNK_SIZE; i += 4) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j += 4) {
            if (rand() % 100 < 40 && this->cells[y | (j + 1)].isWall == 0) {
                for (int k = 0; k < 2; k++) {
                    this->cells[y | (j + k + 1)].fire = 1;
                }
            }
            if (rand() % 100 < 40 && this->cells[((i + 1) << CHUNK_SHIFT) | j].isWall == 0) {
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
            if (rand() % 100 < 5) this->cells[y | j].spike = 1;
        }
    }
}

static void generateCoins(Chunk* this) {
    if (this->isStructure) return;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        int y = i << CHUNK_SHIFT;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            if (this->cells[y | j].isWall) continue;
            if (rand() & 1) this->cells[y | j].coin = 1;
        }
    }
}

static void generateFragement(Chunk* this){
    if (!this->fragment) return;
    int x, y;
    do {
        x = rand() & CHUNK_MASK;
        y = rand() & CHUNK_MASK;
    } while(this->cells[(y << CHUNK_SHIFT) | x].isWall);

    this->cells[(y << CHUNK_SHIFT) | x].fragment = 1;
}

static void generateEnemies(Chunk* this) {
    if (this->isStructure || this->isBorder) return;
    int totalEnemies = rand() % (MAX_ENEMIES_PER_CHUNK + 1);
    static const int range = CHUNK_SIZE >> 1;
    LinkedList* enemies = this->enemies;
    for (int i = 0; i < totalEnemies; i++) {
        int cx, cy, x, y;
        do {
            cx = ((rand() % range) << 1) | 1;
            cy = ((rand() % range) << 1) | 1;
            x = cx + (this->x << CHUNK_SHIFT);
            y = cy + (this->y << CHUNK_SHIFT);
        } while (this->cells[(cy << CHUNK_SHIFT) | cx].isWall);
        Enemy* e =
            new_Enemy(x, y, this->cells[(cy << CHUNK_SHIFT) | cx].biomeType);
        enemies->addLast(enemies, e);
    }
}

static void generate(Chunk* this) {
    srand(SEED * this->y + this->x);
    preLoad(this);
    generateBiomeTransition(this);
    generateBiome(this);
    generateTemple(this);
    generateFont(this);
    generateWalls1x1(this);
    generateWalls2x2(this);
    generateWalls3x3(this);
    generateBorder(this);
    generateWind(this);
    generateMud(this);
    generateGraves(this);
    generateFire(this);
    generateSpikes(this);
    generateCoins(this);
    generateFragement(this);
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