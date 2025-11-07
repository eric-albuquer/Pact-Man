#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "enemy.h"
#include "map.h"

#define MAX_PERSUIT_RADIUS 30
#define BEST_PATH_PROBABILITY 50

void mapDistancePlayer(Map* map);

bool enemyStepTowardsPlayer(Map* map, Enemy* e);

#endif
