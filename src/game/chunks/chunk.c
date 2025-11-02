#include "chunk.h"
#include <stdlib.h>
#include "enemy.h"

static Cell createCell(bool isWall, int biomeType) { return (Cell){isWall, biomeType, -1}; }

static void resetDistance(Chunk* this){
    Cell* cells = this->cells;
    for (int i = 0; i < CELLS_PER_CHUNK; i++){
        cells[i].distance = -1;
    }
}

static void generateEnemies(Chunk* this) {
    int totalEnemies = rand() % (MAX_ENIMIES_PER_CHUNK + 1);
    static const int range = CHUNK_SIZE >> 1;
    LinkedList* enemies = this->enemies;
    for (int i = 0; i < totalEnemies; i++) {
        int cx = (rand() % range) * 2 + 1;
        int cy = (rand() % range) * 2 + 1;
        int x = cx + this->x * CHUNK_SIZE;
        int y = cy + this->y * CHUNK_SIZE;
        Enemy* e = new_Enemy(x, y, this->cells[cy * CHUNK_SIZE + cx].biomeType);
        enemies->addLast(enemies, e);
    }
}

static void generateMap(Chunk* this) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            bool evenOrEven = (y % 2 == 0) || (x % 2 == 0);
            this->cells[y * CHUNK_SIZE + x].isWall = evenOrEven;
        }
    }

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            if ((y % 2 == 0) && (x % 2 == 0)) continue;
            if (rand() % 100 <= 65) this->cells[y * CHUNK_SIZE + x].isWall = 0;
        }
    }
}

static void generateBiome(Chunk* this, int seed){
    const int NOISE_BASE = 112512513 ^ seed;
    static const int BASE_X = CHUNK_SIZE * 3;
    int dx0 = ((this->y * NOISE_BASE) % 11) - 5;
    int x0 = BASE_X + dx0;
    int dx1 = (((this->y + 1) * NOISE_BASE) % 11) - 5;
    int x1 = BASE_X + dx1;

    float delta = x1 - x0;
    
    for (int i = 0; i < CHUNK_SIZE; i++){
        float t = i / (float)CHUNK_SIZE;
        int dx = delta * t;
        int x = x0 + dx;
        for (int j = 0; j < CHUNK_SIZE; j++){
            int chunkX = this->x * CHUNK_SIZE + j;
            if (chunkX < x) 
                this->cells[i * CHUNK_SIZE + j] = createCell(0, 1);
            else
                this->cells[i * CHUNK_SIZE + j] = createCell(0, 2);
        }
    }
}

static void generate(Chunk* this, int seed) {
    generateBiome(this, seed);
    generateEnemies(this);
    generateMap(this);
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

Chunk* new_Chunk(int x, int y, int seed) {
    Chunk* this = malloc(sizeof(Chunk));
    this->x = x;
    this->y = y;
    this->enemies = new_LinkedList();
    generate(this, seed);

    this->resetDistance = resetDistance;
    this->free = _free;
    return this;
}