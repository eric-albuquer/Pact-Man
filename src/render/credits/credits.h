#ifndef CREDITS_H
#define CREDITS_H

#include "audio.h"
#include "sprites.h"
#include "button.h"

typedef struct Credits{
    int width;
    int height;

    Animation* animations;
    Sprite* sprites;
    Audio* audio;

    Button* cutscenePrev;
    Button* cutsceneNext;

    void (*draw)(struct Credits*);
    void (*update)(struct Credits*);
    void (*free)(struct Credits*);
} Credits;

Credits* new_Credits(int width, int height);

#endif