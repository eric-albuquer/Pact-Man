#ifndef RENDER_H
#define RENDER_H

#include <raylib.h>
#include "menu.h"
#include "game.h"
#include "credits.h"
#include "map.h"
#include "common.h"

typedef struct Render {
    int width;
    int height;

    Menu* menu;
    Game* game;
    Credits* credits;

    void (*drawMenu)(struct Render*);
    void (*drawGame)(struct Render*);
    void (*drawCredits)(struct Render*);
    
    void (*updateMenu)(struct Render*);
    void (*saveGame)(struct Render*);
    void (*updateGame)(struct Render*);
    void (*updateCredits)(struct Render*);

    void (*free)(struct Render*);
} Render;

Render* new_Render(int width, int height, int cellSize, Map* map);

#endif