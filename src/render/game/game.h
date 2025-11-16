#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include "audio.h"
#include "sprites.h"
#include "map.h"

#define DELTA_VOLUME 0.01f
#define FRAMES_ANIMATION 8.0f
#define HALF_FRAMES_ANIMATION 4.0f

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
    unsigned int updateCount;

    Map* map;

    Audio* audio;

    RenderTexture2D shadowMap;

    Animation *animations;
    Sprite *sprites;

    void (*draw)(struct Game*);
    void (*saveUpdate)(struct Game*);
    void (*free)(struct Game*);
} Game;

Game* new_Game(int width, int height, int cellSize, Map* map);

#endif