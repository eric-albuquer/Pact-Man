#include "controler.h"
#include "map.h"
#include "raylib.h"
#include "render.h"
#include "time.h"
#include <stdlib.h>

#define UPDATE_TIME 0.20f

int main(void) {
    InitWindow(1920, 1080, "Pact-Man");
    SetTargetFPS(60);

    ChangeDirectory(GetApplicationDirectory());

    srand(time(NULL));

    Render* render = new_Render(1920, 1080, 50);

    Controler* controler = new_Controler();
    Map* map = new_Map(299, 299, 20);

    float lastTime = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        lastTime += dt;
        controler->getInputs(controler);
        if (lastTime >= UPDATE_TIME) {
            lastTime = 0;
            map->update(map, controler);
            controler->reset(controler);
            render->saveUpdate(render);
        }
        BeginDrawing();
        render->drawMapDebug(render, map);
        EndDrawing();
    }

    map->free(map);
    render->free(render);
    controler->free(controler);

    CloseWindow();
    return 0;
}
