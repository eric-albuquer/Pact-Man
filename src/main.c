#include <stdlib.h>

#include "controler.h"
#include "map.h"
#include "raylib.h"
#include "render.h"
#include "time.h"
#include "common.h"
#include <stdio.h>

#define UPDATE_TIME 0.20f

GameState state = MENU;
Controler* controler;
Map* map;
Render* render;

void updateGame() {
    static float lastTime = 0;
    lastTime += GetFrameTime();
    controler->getInputs(controler);

    if (lastTime >= UPDATE_TIME) {
        lastTime = 0;
        map->update(map, controler);
        controler->reset(controler);
        render->updateGame(render);
    }

    BeginDrawing();
    render->drawGame(render);
    EndDrawing();
}

void updateMenu() {
    render->updateMenu(render);

    BeginDrawing();
    render->drawMenu(render);
    EndDrawing();
}

int main(void) {
    InitWindow(1920, 1080, "Pact-Man");
    SetTargetFPS(60);

    ChangeDirectory(GetApplicationDirectory());

    controler = new_Controler();
    map = new_Map(30, 20);
    render = new_Render(1920, 1080, 50, map);

    while (!WindowShouldClose()) {
        if (state == GAME) updateGame();
        else updateMenu();
    }

    map->free(map);
    render->free(render);
    controler->free(controler);

    CloseWindow();
    return 0;
}
