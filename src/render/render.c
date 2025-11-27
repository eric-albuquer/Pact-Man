#include "render.h"
#include <raylib.h>
#include <stdlib.h>

//===============================================================
//  INTERFACE DE FUNÇÕES DE ATUALIZAÇÃO DO RENDER
//===============================================================

static inline void updateMenu(Render* this) { this->menu->update(this->menu); }

static inline void saveGame(Render* this) { this->game->saveUpdate(this->game); }

static inline void updateGame(Render* this) { this->game->update(this->game); }

static inline void updateCredits(Render* this) { this->credits->update(this->credits); }

//===============================================================
//  INTERFACE DE FUNÇÕES DE DESENHO DO RENDER
//===============================================================

static inline void drawMenu(Render* this) { this->menu->draw(this->menu); }

static inline void drawGame(Render* this) { this->game->draw(this->game); }

static inline void drawCredits(Render* this) { this->credits->draw(this->credits); }

//===============================================================
//  LIBERAR MEMÓRIA
//===============================================================

static void _free(Render* this) {
    this->game->free(this->game);
    this->menu->free(this->menu);
    this->credits->free(this->credits);
    free(this);
}

//===============================================================
//  CONSTRUTOR DA CLASSE
//===============================================================

Render* new_Render(int width, int height, int cellSize, Map* map) {
    Render* this = malloc(sizeof(Render));
    this->width = width;
    this->height = height;

    this->menu = new_Menu(width, height);
    this->game = new_Game(width, height, cellSize, map);
    this->credits = new_Credits(width, height, &map->lastScore);

    this->drawMenu = drawMenu;
    this->drawGame = drawGame;
    this->drawCredits = drawCredits;

    this->updateMenu = updateMenu;
    this->saveGame = saveGame;
    this->updateGame = updateGame;
    this->updateCredits = updateCredits;

    this->free = _free;
    return this;
}