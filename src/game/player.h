#ifndef PLAYER_H
#define PLAYER_H

typedef struct Player {
    int x;
    int y;

    int lastX;
    int lastY;

    void (*free)(struct Player*);
} Player;

Player* new_Player(int x, int y);

#endif