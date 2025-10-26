#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "controler.h"
#include "enemy.h"
#include "linkedlist.h"

static Cell createCell(bool isWall) { return (Cell){isWall}; }

static void update(Map* this, LinkedList* ioBuffer) {
    while (ioBuffer->length > 0){
        Input* input = ioBuffer->removeFirst(ioBuffer);
        int nx = this->player->x + input->dx;
        int ny = this->player->y + input->dy;
        free(input);
        if (nx >= 0 && nx < this->cols && ny >= 0 && ny < this->rows &&
            !this->matrix[ny][nx].isWall) {
            this->player->lastX = this->player->x;
            this->player->lastY = this->player->y;
            this->player->x = nx;
            this->player->y = ny;
            break;
        }
    };
}

static void loadChunks(Map* this, unsigned int chunkSize) {
    int chunkRows = ceil(this->rows / (float)chunkSize);
    int chunkCols = ceil(this->cols / (float)chunkSize);
    int totalChunks = chunkRows * chunkCols;
    HashTable* chunks = new_HashTable(totalChunks);
    this->chunks = chunks;

    for (int i = 0; i < chunkRows; i++) {
        for (int j = 0; j < chunkCols; j++) {
            LinkedList* list = new_LinkedList();
            char key[100];
            sprintf(key, "%d,%d", i, j);
            chunks->set(chunks, key, list);
        }
    }
}

void mazeGen(Map* map) {
    unsigned int rows = map->rows;
    unsigned int cols = map->cols;

    for (unsigned int y = 0; y < rows; y++) {
        for (unsigned int x = 0; x < cols; x++) {
            map->matrix[y][x] = createCell(0);
        }
    }

    for (unsigned int x = 0; x < cols; x++) {
        map->matrix[0][x].isWall = 1;
        map->matrix[rows - 1][x].isWall = 1;
    }
    for (unsigned int y = 0; y < rows; y++) {
        map->matrix[y][0].isWall = 1;
        map->matrix[y][cols - 1].isWall = 1;
    }

    srand(time(NULL));

    int step = 2;
    for (int y = 1; y < (int)rows - 1; y += step) {
        for (int x = 1; x < (int)cols - 1; x += step) {
            if (rand() % 100 < 80) {
                map->matrix[y][x].isWall = 1;

                int dir = rand() % 4;
                int nx = x;
                int ny = y;
                switch (dir) {
                    case 0:
                        ny = y - 1;
                        break;  // cima
                    case 1:
                        ny = y + 1;
                        break;  // baixo
                    case 2:
                        nx = x - 1;
                        break;  // esquerda
                    case 3:
                        nx = x + 1;
                        break;  // direita
                }
                if (nx > 0 && nx < (int)cols - 1 && ny > 0 &&
                    ny < (int)rows - 1)
                    map->matrix[ny][nx].isWall = 1;
            }
        }
    }

    for (int i = 0; i < (int)(rows * cols / 200); i++) {
        int x = 1 + rand() % (cols - 2);
        int y = 1 + rand() % (rows - 2);
        map->matrix[y][x].isWall = 0;
    }
}

static void biomeGen(Map* this) { return; }

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

    this->player = new_Player(10, 10);
    this->update = update;
    this->free = _free;
    return this;
}