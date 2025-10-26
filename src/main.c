#include "controler.h"
#include "map.h"
#include "raylib.h"
#include "render.h"

int main(void) {
    InitWindow(1920, 1080, "Pact-Man");
    SetTargetFPS(60);

    Render* render = new_Render(1920, 1080, 60, 50);
    Controler* controler = new_Controler();
    Map* map = new_Map(3000, 1000, 30);

    float lastTime = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        lastTime += dt;
        controler->getInputs(controler);
        if (lastTime >= 0.15) {
            lastTime = 0;
            map->update(map, controler->dx, controler->dy);
            controler->reset(controler);
        }
        BeginDrawing();
        render->drawMap(render, map);
        EndDrawing();
    }

    map->free(map);
    render->free(render);
    controler->free(controler);

    CloseWindow();
    return 0;
}
