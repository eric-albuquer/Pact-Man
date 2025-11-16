#include <stdlib.h>

#include "controler.h"
#include "map.h"
#include "raylib.h"
#include "render.h"
#include "common.h"
#include <stdio.h>

#define UPDATE_TIME 0.15f

GameState state = MENU_MAIN_CONTENT;
Controler* controler;
Map* map;
Render* render;
Font InfernoFont;

void updateGame() {
    static float lastTime = 0;
    lastTime += GetFrameTime();
    controler->getInputs(controler);

    if (lastTime >= UPDATE_TIME) {
        lastTime = 0;
        map->update(map, controler);
        controler->reset(controler);
        render->saveGame(render);
    }

    render->updateGame(render);
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

void updateCredits() {
    render->updateCredits(render);

    BeginDrawing();
    render->drawCredits(render);
    EndDrawing();
}

int main(void) {
    InitWindow(1920, 1080, "Pact-Man");
    //ToggleFullscreen();
    InitAudioDevice();
    SetTargetFPS(60);

    ChangeDirectory(GetApplicationDirectory());

    InfernoFont = LoadFont("assets/fonts/Berani.ttf");
    Image icon = LoadImage("assets/sprites/icon.png");   
    SetWindowIcon(icon);
    UnloadImage(icon);  

    controler = new_Controler();
    map = new_Map(5, 9, 11, 21);
    render = new_Render(GetScreenWidth(), GetScreenHeight(), 50, map);

    while (!WindowShouldClose()) {
        if (state >= GAME_MAIN_CONTENT && state <= GAME_DEATH) updateGame();
        else if (state >= MENU_MAIN_CONTENT && state <= MENU_CUTSCENE5) updateMenu();
        else if (state >= CREDITS_CUTSCENE1 && state <= CREDITS_FINAL) updateCredits();
    }

    map->free(map);
    render->free(render);
    controler->free(controler);

    UnloadFont(InfernoFont);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
