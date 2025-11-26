#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include "audio.h"
#include "sprites.h"
#include "map.h"
#include "button.h"

#define DELTA_VOLUME 0.01f
#define FRAMES_ANIMATION 8.0f
#define HALF_FRAMES_ANIMATION 4.0f

#define TUTORIAL_DURATION 15.0f

#define FRAGMENT_EFFECT_DURATION 3.0f

typedef struct Game {
    int width;
    int height;

    int cellSize;

    int renderDistX;
    int renderDistY;

    int offsetHalfX;
    int offsetHalfY;
    int halfDiagonal;

    unsigned int lastUpdate;
    unsigned int frameCount;

    Map* map;

    Audio* audio;

    RenderTexture2D shadowMap;

    Animation *animations;
    Sprite *sprites;

    Button* backMenu;
    Button* restartGame;

    char tutorialText[CELL_COUNT][200];
    float tutorialDuration[CELL_COUNT];

    void (*draw)(struct Game*);
    void (*update)(struct Game*);
    void (*saveUpdate)(struct Game*);
    void (*free)(struct Game*);
} Game;

Game* new_Game(int width, int height, int cellSize, Map* map);

#endif