#include "render.h"
#include <raylib.h>
#include <stdlib.h>

static void drawMap(Render* this, Map* map) {
    ClearBackground(RAYWHITE);
    int px = map->player->x;
    int py = map->player->y;

    for (int i = -this->renderDist; i <= this->renderDist; i++) {
        int yIdx = i + py;
        if (yIdx < 0 || yIdx >= map->rows) continue;

        for (int j = -this->renderDist; j <= this->renderDist; j++) {
            int xIdx = j + px;
            if (xIdx < 0 || xIdx >= map->cols) continue;

            Color color = map->matrix[yIdx][xIdx].isWall
                ? (Color){127, 127, 127, 255}
                : (Color){0, 0, 0, 255};

            DrawRectangle(
                this->offsetHalfX + j * this->cellSize,
                this->offsetHalfY + i * this->cellSize,
                this->cellSize,
                this->cellSize,
                color
            );
        }
    }

    // desenha o player fixo no centro
    DrawRectangle(
        this->offsetHalfX,
        this->offsetHalfY,
        this->cellSize,
        this->cellSize,
        (Color){255, 255, 255, 255}
    );
    this->frameCount++;
}

void _free(Render* this){
    free(this);
}

Render* new_Render(int width, int height, int cellSize, int renderDist) {
    Render* render = malloc(sizeof(Render));
    render->width = width;
    render->height = height;
    render->cellSize = cellSize;
    render->renderDist = renderDist;
    render->frameCount = 0;

    render->offsetHalfX = width >> 1;
    render->offsetHalfY = height >> 1;

    render->drawMap = drawMap;
    render->free = _free;
    return render;
}