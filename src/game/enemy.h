#ifndef ENEMY_H
#define ENEMY_H

typedef struct Enemy {
    float x;
    float y;

    void (*free)(Enemy*);
} Enemy;

Enemy* new_Enemy(float x, float y);

#endif