#ifndef ENEMY_H
#define ENEMY_H

typedef struct Enemy {
    int x;
    int y;

    void (*free)(struct Enemy*);
} Enemy;

Enemy* new_Enemy(int x, int y);

#endif