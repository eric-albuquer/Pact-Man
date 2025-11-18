#ifndef MENU_H
#define MENU_H

#include <raylib.h>
#include "button.h"
#include "audio.h"
#include "sprites.h"

#define CUTSCENE_IMAGE_TIME 10.0f

typedef struct Menu {
    Button* play;
    Button* tutorial;
    Button* volume;
    Button* difficulty;

    Button* cutscenePrev;
    Button* cutsceneNext;

    Button* credits;
    Button* score;

    //Animation* animations;
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

#define NUM_FLAMES 100

Menu* new_Menu(int width, int height);

#endif

