#include "map.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "linkedlist.h"
#include "enemy.h"

static Cell createCell(bool isWall) { return (Cell){isWall, 0}; }

static void update(Map* this) {
    this->player->x += 0.05;
    this->player->y += 0.02;
}

static void loadChunks(Map* this, uInt64 chunkSize) {
    int chunkArea = chunkSize * chunkSize;
    HashTable* chunks = new_HashTable((this->rows * this->cols) / chunkArea);
    this->chunks = chunks;

    int chunkRows = ceil(this->rows / (float)chunkSize);
    int chunkCols = ceil(this->cols / (float)chunkSize);
    for (int i = 0; i < chunkRows; i++) {
        for (int j = 0; j < chunkCols; j++) {
            LinkedList* list = new_LinkedList();
            char key[100];
            sprintf(key, "%d,%d", i, j);
            chunks->set(chunks, key, list);
        }
    }
}

static void mazeGen(Map* this) {
    for (uInt64 i = 0; i < this->rows; i++) {
        for (uInt64 j = 0; j < this->cols; j++) {
            this->matrix[i][j] = createCell((i & 1) && (j & 1));
        }
    }
}

static void biomeGen(Map* this) { return; }

static void _free(Map* this){
    HashNode* cur = this->chunks->keys->head;
    while(cur != NULL){
        LinkedList* list = cur->data;
        Node* curList = list->head;
        Node* temp;
        while(curList != NULL){
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

    for (uInt64 i = 0; i < this->rows; i++){
        free(this->matrix[i]);
    }
    free(this->matrix);
    Player* p = this->player;
    p->free(p);
}

Map* new_Map(uInt64 rows, uInt64 cols, uInt64 chunkSize) {
    Map* this = malloc(sizeof(Map));
    this->rows = rows;
    this->cols = cols;
    this->matrix = malloc(sizeof(Cell*) * rows);
    for (uInt64 i = 0; i < rows; i++) {
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