#include "render.h"
#include <raylib.h>
#include <stdlib.h>

static void drawMap(Render* this, Map* map) {
    int px = (int)map->player->x;
    int py = (int)map->player->y;

    float subcellX = map->player->x - px;
    float subcellY = map->player->y - py;

    float centerX = this->offsetHalfX - subcellX * this->cellSize;
    float centerY = this->offsetHalfY - subcellY * this->cellSize;

    for (int i = -this->renderDist; i <= this->renderDist; i++) {
        int yIdx = py + i;
        if (yIdx < 0 || yIdx >= map->rows) continue;

        for (int j = -this->renderDist; j <= this->renderDist; j++) {
            int xIdx = px + j;
            if (xIdx < 0 || xIdx >= map->cols) continue;

            Color color = map->matrix[yIdx][xIdx].isWall
                              ? (Color){127, 127, 127, 255}
                              : (Color){0, 0, 0, 255};

            DrawRectangle(
                centerX + j * this->cellSize,
                centerY + i * this->cellSize,
                this->cellSize,
                this->cellSize,
                color
            );
        }
    }
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

    render->offsetHalfX = width >> 1;
    render->offsetHalfY = height >> 1;

    render->drawMap = drawMap;
    render->free = _free;
    return render;
}