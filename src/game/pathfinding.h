#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "enemy.h"
#include "map.h"

typedef struct {
    int x;
    int y;
} Vec2i;

typedef struct QNode {
    int x, y, d;
} QNode;

typedef struct {
    QNode pos[4];
    int moves;
} NextPos;

void mapDistancePlayer(Map* map);

NextPos getNextPos(ChunkManager* cm, int x, int y, int biome);

void sortNextPos(NextPos* nextPos);
Vec2i getWorstPos(NextPos nextPos);
Vec2i getBestPos(NextPos nextPos);
Vec2i getRandomPos(NextPos nextPos);

#endif
