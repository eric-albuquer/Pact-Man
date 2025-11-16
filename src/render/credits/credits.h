#ifndef CREDITS_H
#define CREDITS_H

#include "audio.h"
#include "sprites.h"
#include "button.h"
#include <stdio.h>
#include "linkedlist.h"
#include "arraylist.h"
#include "player.h"

typedef struct Credits {
    int width;
    int height;

    Animation* animations;
    Sprite* sprites;
    Audio* audio;

    FILE* file;

    Button* prevBtn;
    Button* nextBtn;

    Button* sortByCoins;
    Button* sortByFragments;
    Button* sortByTime;

    LinkedList* lines;
    ArrayList* scores;

    char name[100];
    int nameIdx;

    Player* player;

    unsigned int updateCount;
    bool creditsEnd;

    void (*draw)(struct Credits*);
    void (*update)(struct Credits*);
    void (*free)(struct Credits*);
} Credits;

Credits* new_Credits(int width, int height, Player* p);

#endif