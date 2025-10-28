#ifndef RENDER_H
#define RENDER_H

#include "map.h"

#define FRAMES_ANIMATION 8.0f

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

    void (*drawMapDebug)(struct Render*, Map*);
    void (*saveUpdate)(struct Render*);
    void (*free)(struct Render*);
} Render;

Render* new_Render(int width, int height, int cellSize);

#endif