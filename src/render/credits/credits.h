#ifndef CREDITS_H
#define CREDITS_H

#include "audio.h"
#include "sprites.h"
#include "button.h"
#include <stdio.h>
#include "linkedlist.h"

typedef struct Credits {
    int width;
    int height;

    Animation* animations;
    Sprite* sprites;
    Audio* audio;

    FILE* file;

    Button* cutscenePrev;
    Button* cutsceneNext;

    LinkedList* creditsLines;

    unsigned int updateCount;
    bool creditsEnd;

    void (*draw)(struct Credits*);
    void (*update)(struct Credits*);
    void (*free)(struct Credits*);
} Credits;

Credits* new_Credits(int width, int height);

#endif