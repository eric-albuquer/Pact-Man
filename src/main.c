#include "raylib.h"
#include "render.h"

int main(void) {
    InitWindow(1920, 1080, "Teste Raylib");
    SetTargetFPS(60);
    
    Render* render = new_Render(1920, 1080, 50, 30);
    Map* map = new_Map(1000, 1000);

    while (!WindowShouldClose()) {
        map->update(map);
        BeginDrawing();
            ClearBackground(RAYWHITE);
            render->drawMap(render, map);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
