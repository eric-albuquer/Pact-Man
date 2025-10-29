#include "controler.h"
#include "map.h"
#include "raylib.h"
#include "render.h"
#include "time.h"
#include <stdlib.h>

static const char* SPRITES[] = {
    "assets/sprites/ghostRight1.png",
    "assets/sprites/ghostRight2.png",
    "assets/sprites/ghostDown1.png",
    "assets/sprites/ghostDown2.png",
    "assets/sprites/ghostLeft1.png",
    "assets/sprites/ghostLeft2.png",
    "assets/sprites/ghostUp1.png",
    "assets/sprites/ghostUp2.png",
    
    "assets/sprites/pacmanRight1.png",
    "assets/sprites/pacmanRight2.png",
    "assets/sprites/pacmanDown1.png",
    "assets/sprites/pacmanDown2.png",
    "assets/sprites/pacmanLeft1.png",
    "assets/sprites/pacmanLeft2.png",
    "assets/sprites/pacmanUp1.png",
    "assets/sprites/pacmanUp2.png",
};

int main(void) {
    InitWindow(1920, 1080, "Pact-Man");
    SetTargetFPS(60);

    ChangeDirectory(GetApplicationDirectory());

    srand(time(NULL));

    Render* render = new_Render(1920, 1080, 35, SPRITES, 16);

    Controler* controler = new_Controler();
    Map* map = new_Map(301, 301, 20);

    float lastTime = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        lastTime += dt;
        controler->getInputs(controler);
        if (lastTime >= 0.15) {
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
