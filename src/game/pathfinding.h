#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "map.h"
#include "enemy.h"

#define MAX_PERSUIT_RADIUS 30
#define BEST_PATH_PROBABILITY 0.8f

void mapDistancePlayer(Map *map, int max_distance);

bool enemyStepTowardsPlayer(Map *map, Enemy *e);

#endif
