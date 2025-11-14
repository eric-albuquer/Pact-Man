#ifndef MENU_H
#define MENU_H

#include <raylib.h>
#include "button.h"
#include "audio.h"
#include "sprites.h"

typedef struct Menu {
    Button* play;
    Button* tutorial;
    Button* volume;
    Button* difficulty;

    Animation* animations;
    Sprite* sprites;

    Audio* audio;

    int width;
    int height;

    void (*draw)(struct Menu*);
    void (*update)(struct Menu*);
    void (*free)(struct Menu*);
} Menu;

typedef struct FlameParticle {
    float x;
    float y;
    float speed;
    float radius;
    Color color;
} FlameParticle;

#define NUM_FLAMES 60

Menu* new_Menu(int width, int height);

#endif

