#include "render.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "enemy.h"

static void drawMap(Render* this, Map* map) {
    ClearBackground(RAYWHITE);
    int px = map->player->x;
    int py = map->player->y;
    static char buffer[100];

    static const Color colors[4] = {
        (Color){100, 0, 0, 255},
        (Color){160, 0, 0, 255},
        (Color){0, 100, 0, 255},
        (Color){0, 0, 150, 255},
    };

    for (int i = -this->renderDist; i <= this->renderDist; i++) {
        int yIdx = i + py;
        if (yIdx < 0 || yIdx >= map->rows) continue;

        for (int j = -this->renderDist; j <= this->renderDist; j++) {
            int xIdx = j + px;
            if (xIdx < 0 || xIdx >= map->cols) continue;

            int biomeType = map->matrix[yIdx][xIdx].biomeType;
            Color color = colors[biomeType - 1];

            if (map->matrix[yIdx][xIdx].isWall) {
                color.r += 50;
                color.g += 50;
                color.b += 50;
            }

            int x = this->offsetHalfX + j * this->cellSize;
            int y = this->offsetHalfY + i * this->cellSize;

            DrawRectangle(x, y, this->cellSize, this->cellSize, color);

            sprintf(buffer, "%d", map->matrix[yIdx][xIdx].distance);
            DrawText(buffer, x + 5, y + 5, 20, (Color){255, 255, 255, 255});
        }
    }

    HashTable* chunks = map->chunks;
    for (int i = -1; i < 2; i++) {
        int chunkY = map->player->chunkY + i;
        if (chunkY < 0 || chunkY >= map->chunkRows) continue;
        for (int j = -1; j < 2; j++) {
            int chunkX = map->player->chunkX + j;
            if (chunkX < 0 || chunkX >= map->chunkCols) continue;
            sprintf(buffer, "%d,%d", chunkY, chunkX);
            LinkedList* enemies = chunks->get(chunks, buffer);
            Node* cur = enemies->head;
            while (cur != NULL) {
                Enemy* e = cur->data;
                int x = this->offsetHalfX + (e->x - px) * this->cellSize;
                int y = this->offsetHalfY + (e->y - py) * this->cellSize;

                DrawRectangle(x, y, this->cellSize, this->cellSize, (Color){255, 255, 0, 255});
                cur = cur->next;
            }
        }
    }

    // desenha o player fixo no centro
    DrawRectangle(this->offsetHalfX, this->offsetHalfY, this->cellSize,
                  this->cellSize, (Color){255, 255, 255, 255});
    this->frameCount++;
}

void _free(Render* this) { free(this); }

Render* new_Render(int width, int height, int cellSize) {
    Render* render = malloc(sizeof(Render));
    render->width = width;
    render->height = height;
    render->cellSize = cellSize;
    render->renderDist = ((width / cellSize) >> 1) + 1;
    render->frameCount = 0;

    render->offsetHalfX = width >> 1;
    render->offsetHalfY = height >> 1;

    render->drawMap = drawMap;
    render->free = _free;
    return render;
}