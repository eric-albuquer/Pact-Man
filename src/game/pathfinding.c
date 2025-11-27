#include "pathfinding.h"

#include <math.h>
#include <stdlib.h>

#include "chunk.h"
#include "linkedlist.h"

static LinkedList* BFSQueue = NULL;

//===============================================================
//  ALOCAR E LIBERAR FILA BFS
//===============================================================

void loadBFS(){
    BFSQueue = new_LinkedList();
}

void unloadBFS(){
    Node* cur = BFSQueue->head;
    while(cur){
        free(cur->data);
        cur = cur->next;
    }
    BFSQueue->free(BFSQueue);
}

//===============================================================
//  ALGORITMO BFS PARA GERAR MAPA DE DISTÂNCIA
//===============================================================

void mapDistancePlayer(Map* map) {
    if (!map) return;

    ChunkManager* cm = map->manager;
    Chunk** adjacents = cm->adjacents;

    for (int i = 0; i < 9; i++) {
        int idx = CLOSER_IDX[i];
        Chunk* chunk = adjacents[idx];
        if (chunk) chunk->resetDistance(chunk);
    }

    QNode* start = malloc(sizeof(*start));

    start->x = map->player->x;
    start->y = map->player->y;
    start->d = 0;

    BFSQueue->addLast(BFSQueue, start);
    Cell* cell = cm->getUpdatedCell(cm, start->x, start->y);
    cell->distance = 0;

    static const int DX[4] = { 1, -1, 0, 0 };
    static const int DY[4] = { 0, 0, 1, -1 };

    while (BFSQueue->length > 0) {
        QNode* curr = BFSQueue->removeFirst(BFSQueue);
        for (int i = 0; i < 4; i++) {
            int neighborX = curr->x + DX[i];
            int neighborY = curr->y + DY[i];

            Cell* next = cm->getUpdatedCell(cm, neighborX, neighborY);
            if (!next || !isPassable(next->type) || next->distance != -1) continue;

            next->distance = curr->d + 1;
            QNode* neighborNew = malloc(sizeof(*neighborNew));
            neighborNew->x = neighborX;
            neighborNew->y = neighborY;
            neighborNew->d = curr->d + 1;
            BFSQueue->addLast(BFSQueue, neighborNew);
        }
        free(curr);
    }
}

//===============================================================
//  POSIÇÕES VIZINHAS PARA UMA CORDENADA
//===============================================================

NextPos getNextPos(ChunkManager* cm, int x, int y, int biome) {
    NextPos nextPos = { 0 };
    if (cm == NULL) return nextPos;

    Cell* cell = cm->getUpdatedCell(cm, x, y);
    if (!cell || cell->distance < 0) return nextPos;

    static const int DX[4] = { 0, 1, 0, -1 };
    static const int DY[4] = { -1, 0, 1, 0 };

    for (int i = 0; i < 4; i++) {
        int neighborX = x + DX[i];
        int neighborY = y + DY[i];

        Cell* next = cm->getUpdatedCell(cm, neighborX, neighborY);

        if (next && isPassable(next->type) && next->biome == biome)
            nextPos.pos[nextPos.moves++] = (QNode){ neighborX, neighborY, next->distance };
    }

    return nextPos;
}

//===============================================================
//  ORDENAR POSIÇÕES VIZINHAS PELA DISTÂNCIA
//===============================================================

void sortNextPos(NextPos* nextPos) {
    int n = nextPos->moves;

    if (n < 2) return;

    QNode* p = nextPos->pos;

    if (n == 2) {
        if (p[0].d > p[1].d) {
            QNode tmp = p[0]; p[0] = p[1]; p[1] = tmp;
        }
        return;
    }

    if (n == 3) {
        if (p[0].d > p[1].d) { QNode t = p[0]; p[0] = p[1]; p[1] = t; }
        if (p[1].d > p[2].d) { QNode t = p[1]; p[1] = p[2]; p[2] = t; }
        if (p[0].d > p[1].d) { QNode t = p[0]; p[0] = p[1]; p[1] = t; }
        return;
    }

    if (n == 4) {
        if (p[0].d > p[1].d) { QNode t = p[0]; p[0] = p[1]; p[1] = t; }
        if (p[2].d > p[3].d) { QNode t = p[2]; p[2] = p[3]; p[3] = t; }
        if (p[0].d > p[2].d) { QNode t = p[0]; p[0] = p[2]; p[2] = t; }
        if (p[1].d > p[3].d) { QNode t = p[1]; p[1] = p[3]; p[3] = t; }
        if (p[1].d > p[2].d) { QNode t = p[1]; p[1] = p[2]; p[2] = t; }
    }
}

//===============================================================
//  PIOR DIREÇÃO
//===============================================================

Vec2i getWorstPos(NextPos nextPos) {
    if (nextPos.moves == 0) return (Vec2i) { 0 };
    return (Vec2i) { nextPos.pos[nextPos.moves - 1].x, nextPos.pos[nextPos.moves - 1].y };
}

//===============================================================
//  MELHOR DIREÇÃO
//===============================================================

Vec2i getBestPos(NextPos nextPos) {
    if (nextPos.moves == 0) return (Vec2i) { 0 };
    return (Vec2i) { nextPos.pos[0].x, nextPos.pos[0].y };
}

//===============================================================
//  DIREÇÃO ALEATÓRIA
//===============================================================

Vec2i getRandomPos(NextPos nextPos) {
    if (nextPos.moves == 0) return (Vec2i) { 0 };
    int idx = rand() % nextPos.moves;
    return (Vec2i) { nextPos.pos[idx].x, nextPos.pos[idx].y };
}

//===============================================================
//  MELHOR DIREÇÃO PARA O SPAWN (EUCLIDIANA)
//===============================================================

Vec2i getCloserToSpawn(NextPos nextPos, int x, int y){
    Vec2i closer = {0};
    if (nextPos.moves == 0) return closer;
    float dist = 10000000.0f;
    for (int i = 0; i < nextPos.moves; i++){
        QNode pos = nextPos.pos[i];
        float posDist = hypotf(pos.x - x, pos.y - y);
        if (posDist < dist){
            dist = posDist;
            closer.x = pos.x;
            closer.y = pos.y;
        }
    }
    return closer;
}