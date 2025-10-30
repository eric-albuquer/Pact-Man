#include "render.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "enemy.h"

static void updateGame(Render* this){
    this->game->saveUpdate(this->game);
}

static void drawGame(Render* this, Map* map){
    this->game->drawMapDebug(this->game, map);
}

static void _free(Render* this) {
    this->game->free(this->game);
    this->menu->free(this->menu);
    free(this);
}

Render* new_Render(int width, int height, int cellSize) {
    Render* this = malloc(sizeof(Render));
    this->width = width;
    this->height = height;

    this->menu = new_Menu();
    this->game = new_Game(width, height, cellSize);

    this->drawGame = drawGame;
    this->updateGame = updateGame;
    this->free = _free;
    return this;
}