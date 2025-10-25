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

    void (*drawMap)(struct Render*, Map*);
} Render;

Render* new_Render(int width, int height, int cellSize, int renderDist);

#endif