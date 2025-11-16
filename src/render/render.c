#include "render.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "enemy.h"

static void updateMenu(Render* this) { this->menu->update(this->menu); }

static void updateGame(Render* this) { this->game->saveUpdate(this->game); }

static void updateCredits(Render* this) { this->credits->update(this->credits); }


static void drawMenu(Render* this) { this->menu->draw(this->menu); }

static void drawGame(Render* this) { this->game->draw(this->game); }

static void drawCredits(Render* this) { this->credits->draw(this->credits); }

static void _free(Render* this) {
    this->game->free(this->game);
    this->menu->free(this->menu);
    free(this);
}

Render* new_Render(int width, int height, int cellSize, Map* map) {
    Render* this = malloc(sizeof(Render));
    this->width = width;
    this->height = height;

    this->menu = new_Menu(width, height);
    this->game = new_Game(width, height, cellSize, map);
    this->credits = new_Credits(width, height);

    this->drawMenu = drawMenu;
    this->drawGame = drawGame;
    this->drawCredits = drawCredits;

    this->updateMenu = updateMenu;
    this->updateGame = updateGame;
    this->updateCredits = updateCredits;

    this->free = _free;
    return this;
}