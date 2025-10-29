#ifndef RENDER_H
#define RENDER_H

#include "map.h"
#include <raylib.h>

#define FRAMES_ANIMATION 10.0f

typedef struct Render {
    int width;
    int height;

    int cellSize;

    int renderDistX;
    int renderDistY;

    int offsetHalfX;
    int offsetHalfY;

    unsigned int lastUpdate;
    unsigned int frameCount;
    unsigned int updateCount;

    Texture2D* sprites;
    unsigned int totalSprites;

    void (*drawMapDebug)(struct Render*, Map*);
    void (*saveUpdate)(struct Render*);
    void (*free)(struct Render*);
} Render;

Render* new_Render(int width, int height, int cellSize, const char** SPRITES,
                   int total);

#endif