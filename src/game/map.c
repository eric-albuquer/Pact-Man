#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "controler.h"
#include "enemy.h"
#include "linkedlist.h"
#include "pathfinding.h"

static Cell createCell(bool isWall) { return (Cell){isWall, 0, 0}; }

static void update(Map* this, LinkedList* inputBuffer) {
    while (inputBuffer->length > 0) {
        Input* input = inputBuffer->removeFirst(inputBuffer);
        int nx = this->player->x + input->dx;
        int ny = this->player->y + input->dy;
        free(input);
        if (nx >= 0 && nx < this->cols && ny >= 0 && ny < this->rows &&
            !this->matrix[ny][nx].isWall) {
            this->player->x = nx;
            this->player->y = ny;
            this->player->updateChunk(this->player, this->chunkSize);
            break;
        }
    };

    mapDistancePlayer(this, this->chunkSize << 1);
    static char key[100];
    HashTable* chunks = this->chunks;

    for (int i = -1; i < 2; i++) {
        int chunkY = this->player->chunkY + i;
        if (chunkY < 0 || chunkY >= this->chunkRows) continue;
        for (int j = -1; j < 2; j++) {
            int chunkX = this->player->chunkX + j;
            if (chunkX < 0 || chunkX >= this->chunkCols) continue;
            sprintf(key, "%d,%d", chunkY, chunkX);
            
            LinkedList* enemies = chunks->get(chunks, key);
            Node* cur = enemies->head;
            while (cur != NULL) {
                Node* next = cur->next;
                Enemy* e = cur->data;
                if (enemyStepTowardsPlayer(this, e) &&
                    e->updateChunk(e, this->chunkSize)) {
                    enemies->removeNode(enemies, cur);
                    printf("Mudando de chunk\n");
                    sprintf(key, "%d,%d", e->chunkY, e->chunkX);
                    LinkedList* enemies2 = chunks->get(chunks, key);
                    enemies2->addFirst(enemies2, e);
                }
                cur = next;
            }
        }
    }
}

static void loadChunks(Map* this, unsigned int chunkSize) {
    this->chunkRows = ceil(this->rows / (float)chunkSize);
    this->chunkCols = ceil(this->cols / (float)chunkSize);
    this->chunkSize = chunkSize;
    int totalChunks = this->chunkRows * this->chunkCols;
    HashTable* chunks = new_HashTable(totalChunks);
    this->chunks = chunks;

    for (int i = 0; i < this->chunkRows; i++) {
        for (int j = 0; j < this->chunkCols; j++) {
            LinkedList* list = new_LinkedList();
            char key[100];
            sprintf(key, "%d,%d", i, j);
            chunks->set(chunks, key, list);
        }
    }
}

static void mazeGen(Map* map) {
    unsigned int rows = map->rows;
    unsigned int cols = map->cols;

    srand(time(NULL));

    for (unsigned int y = 0; y < rows; y++) {
        for (unsigned int x = 0; x < cols; x++) {
            bool evenAndEven = (y % 2 == 0) || (x % 2 == 0);
            map->matrix[y][x] = createCell(evenAndEven);
        }
    }

    for (unsigned int y = 1; y < rows - 1; y++) {
        for (unsigned int x = 1; x < cols - 1; x++) {
            if ((y % 2 == 0) && (x % 2 == 0)) continue;
            if (rand() % 100 <= 65) map->matrix[y][x].isWall = 0;
        }
    }
}

static void biomeGen(Map* this) {
    const int totalBiomes = 4;
    const int deltaX = this->cols / totalBiomes;
    Cell** matrix = this->matrix;
    for (int i = 1; i < totalBiomes; i++) {
        int dx = deltaX * i;
        for (int y = 0; y < this->rows; y++) {
            dx += (rand() % 5) - 2;
            for (int x = dx; x >= 0 && matrix[y][x].biomeType == 0; x--) {
                matrix[y][x].biomeType = i;
            }
        }
    }

    for (int y = 0; y < this->rows; y++) {
        for (int x = this->cols - 1; x >= 0 && matrix[y][x].biomeType == 0;
             x--) {
            matrix[y][x].biomeType = totalBiomes;
        }
    }

    const int range = this->chunkSize >> 1;
    static char key[100];
    HashTable* chunks = this->chunks;
    for (int i = 0; i < this->chunkRows; i++) {
        for (int j = 0; j < this->chunkCols; j++) {
            sprintf(key, "%d,%d", i, j);
            LinkedList* enemies = chunks->get(chunks, key);
            int count = rand() % 3;
            for (int k = 0; k < count; k++) {
                int x = ((rand() % range) * 2) + j * this->chunkSize + 1;
                int y = ((rand() % range) * 2) + i * this->chunkSize + 1;
                if (x >= this->cols || y >= this->rows) continue;
                Enemy* e = new_Enemy(x, y);
                e->updateChunk(e, this->chunkSize);
                enemies->addFirst(enemies, e);
            }
        }
    }
}

static void _free(Map* this) {
    HashNode* cur = this->chunks->keys->head;
    while (cur != NULL) {
        LinkedList* list = cur->data;
        Node* curList = list->head;
        Node* temp;
        while (curList != NULL) {
            temp = curList;
            curList = curList->next;
            Enemy* e = temp->data;
            e->free(e);
            free(temp);
        }
        cur = cur->nextKey;
    }
    HashTable* h = this->chunks;
    h->free(h);

    for (unsigned int i = 0; i < this->rows; i++) {
        free(this->matrix[i]);
    }
    free(this->matrix);
    Player* p = this->player;
    p->free(p);
}

Map* new_Map(unsigned int rows, unsigned int cols, unsigned int chunkSize) {
    Map* this = malloc(sizeof(Map));
    this->rows = rows;
    this->cols = cols;
    this->matrix = malloc(sizeof(Cell*) * rows);
    for (unsigned int i = 0; i < rows; i++) {
        this->matrix[i] = malloc(sizeof(Cell) * cols);
    }

    loadChunks(this, chunkSize);
    mazeGen(this);
    biomeGen(this);

    this->player = new_Player(11, 11);
    this->player->updateChunk(this->player, chunkSize);
    this->update = update;
    this->free = _free;
    return this;
}