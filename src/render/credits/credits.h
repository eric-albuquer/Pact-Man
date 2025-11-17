#ifndef CREDITS_H
#define CREDITS_H

#include "audio.h"
#include "sprites.h"
#include "button.h"
#include "linkedlist.h"
#include "arraylist.h"
#include "player.h"

#define CREDITS_SPEED 0.7f

typedef struct Credits {
    int width;
    int height;

    Animation* animations;
    Sprite* sprites;
    Audio* audio;

    Button* prevBtn;
    Button* nextBtn;

    Button* sortByCoins;
    Button* sortByFragments;
    Button* sortByTime;

    //LinkedList* lines;
    ArrayList* lines;
    ArrayList* scores;

    char name[1000];
    int nameIdx;

    Score* score;

    float creditsMove;
    unsigned int updateCount;

    void (*draw)(struct Credits*);
    void (*update)(struct Credits*);
    void (*free)(struct Credits*);
} Credits;

Credits* new_Credits(int width, int height, Score* score);

#endif