#include <stdlib.h>

#include "controller.h"
#include "map.h"
#include "raylib.h"
#include "render.h"
#include "common.h"
#include <stdio.h>

#define UPDATE_TIME 0.15f

GameState state = MENU_MAIN_CONTENT;
Controller* controller;
Map* map;
Render* render;
Font InfernoFont;
Dificulty dificulty = DIFICULTY_MEDIUM;

void updateGame() {
    static float lastTime = 0;
    lastTime += GetFrameTime();
    controller->getInputs(controller);

    if (lastTime >= UPDATE_TIME) {
        lastTime = 0;
        render->saveGame(render);
        map->update(map, controller, UPDATE_TIME);
        controller->reset(controller);
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

int main(int argc, char *argv[]) {
    //SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    //SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Pact-Men");
    InitAudioDevice();
    SetTargetFPS(60);

    ChangeDirectory(GetApplicationDirectory());

    InfernoFont = LoadFont("assets/fonts/Berani.ttf");
    Image icon = LoadImage("assets/sprites/icon.png");   
    SetWindowIcon(icon);
    UnloadImage(icon);  

    controller = new_Controller();
    map = new_Map(5, 9, 11, 61);
    render = new_Render(GetScreenWidth(), GetScreenHeight(), 50, map);

    while (!WindowShouldClose()) {
        if (state >= GAME_MAIN_CONTENT && state <= GAME_DEATH) updateGame();
        else if (state >= MENU_MAIN_CONTENT && state <= MENU_CUTSCENE5) updateMenu();
        else if (state >= CREDITS_CUTSCENE1 && state <= CREDITS_FINAL) updateCredits();
    }

    map->free(map);
    render->free(render);
    controller->free(controller);

    UnloadFont(InfernoFont);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
