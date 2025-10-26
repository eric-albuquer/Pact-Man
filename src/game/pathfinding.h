#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "map.h"
#include "enemy.h"

void mapDistancePlayer(Map *map, int max_distance);

int enemyStepTowardsPlayer(Map *map, Enemy *e);

#endif
