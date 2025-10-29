#ifndef RENDER_H
#define RENDER_H

#include "map.h"
#include <raylib.h>

#define FRAMES_ANIMATION 9.0f

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

    Texture2D sprites[10];

    void (*drawMapDebug)(struct Render*, Map*);
    void (*saveUpdate)(struct Render*);
    void (*free)(struct Render*);
} Render;

Render* new_Render(int width, int height, int cellSize);

#endif