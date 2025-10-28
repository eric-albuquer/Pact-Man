#ifndef RENDER_H
#define RENDER_H

#include "map.h"

typedef struct Render {
    int width;
    int height;

    int cellSize;

    int renderDistX;
    int renderDistY;

    int offsetHalfX;
    int offsetHalfY;

    unsigned int frameCount;

    void (*drawMapDebug)(struct Render*, Map*);
    void (*free)(struct Render*);
} Render;

Render* new_Render(int width, int height, int cellSize);

#endif