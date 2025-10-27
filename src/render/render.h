#ifndef RENDER_H
#define RENDER_H

#include "map.h"

typedef struct Render {
    int width;
    int height;

    int cellSize;

    int renderDist;

    int offsetHalfX;
    int offsetHalfY;

    unsigned int frameCount;

    void (*drawMap)(struct Render*, Map*);
    void (*free)(struct Render*);
} Render;

Render* new_Render(int width, int height, int cellSize);

#endif