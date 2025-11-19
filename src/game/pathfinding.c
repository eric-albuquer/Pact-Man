#include "pathfinding.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "linkedlist.h"

void mapDistancePlayer(Map* map) {
    if (!map) return;

    ChunkManager* cm = map->manager;
    Chunk** adjacents = cm->adjacents;

    for (int i = 0; i < 9; i++) {
        int idx = CLOSER_IDX[i];
        Chunk* chunk = adjacents[idx];
        if (chunk) chunk->resetDistance(chunk);
    }

    LinkedList* BFSQueue = new_LinkedList();
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
    BFSQueue->free(BFSQueue);
}

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

void sortNextPos(NextPos* nextPos) {
    for (int i = 0; i < nextPos->moves; i++) {
        for (int j = i + 1; j < nextPos->moves; j++) {
            if (nextPos->pos[i].d > nextPos->pos[j].d) {
                QNode temp = nextPos->pos[i];
                nextPos->pos[i] = nextPos->pos[j];
                nextPos->pos[j] = temp;
            }
        }
    }
}

Vec2i getWorstPos(NextPos nextPos) {
    if (nextPos.moves == 0) return (Vec2i) { 0 };
    return (Vec2i) { nextPos.pos[nextPos.moves - 1].x, nextPos.pos[nextPos.moves - 1].y };
}

Vec2i getBestPos(NextPos nextPos) {
    if (nextPos.moves == 0) return (Vec2i) { 0 };
    return (Vec2i) { nextPos.pos[0].x, nextPos.pos[0].y };
}

Vec2i getRandomPos(NextPos nextPos) {
    if (nextPos.moves == 0) return (Vec2i) { 0 };
    int idx = rand() % nextPos.moves;
    return (Vec2i) { nextPos.pos[idx].x, nextPos.pos[idx].y };
}

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