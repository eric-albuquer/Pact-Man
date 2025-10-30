#ifndef RENDER_H
#define RENDER_H

#include <raylib.h>

#include "map.h"

#define FRAMES_ANIMATION 12.0f

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

Render* new_Render(int width, int height, int cellSize);

#endif