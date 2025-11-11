#ifndef SPRITES_H
#define SPRITES_H

#include <raylib.h>

typedef struct Animation {
    Texture2D* frames;
    int lenght;
    int actualFrame;

    void (*free)(struct Animation*);
    void (*draw)(struct Animation*, int, int, int);
    void (*update)(struct Animation*);
    
} Animation;

Animation* new_Animation(int lenght, const char** path);

#endif