#ifndef RENDER_H
#define RENDER_H

#include <raylib.h>
#include "menu.h"
#include "game.h"
#include "map.h"
#include "common.h"

#define FRAMES_ANIMATION 12.0f

typedef struct Render {
    int width;
    int height;

    Menu* menu;
    Game* game;

    void (*drawGame)(struct Render*);
    void (*drawMenu)(struct Render*);
    void (*updateGame)(struct Render*);
    void (*updateMenu)(struct Render*);
    void (*free)(struct Render*);
} Render;

Render* new_Render(int width, int height, int cellSize, Map* map);

#endif