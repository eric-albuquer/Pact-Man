#include "render.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "enemy.h"

static const Color CELL_COLORS[4] = {
    {100, 0, 0, 255}, {160, 0, 0, 255}, {0, 100, 0, 255}, {0, 0, 150, 255}};

static void drawMinimapDebug(Render* this, Map* map, int x0, int y0, int size,
                             int zoom) {
    int cellSize = size / (zoom);
    DrawRectangle(x0 - 5, y0 - 5, size + 10, size + 10, BLACK);
    for (int y = 0; y < zoom; y++) {
        int py = map->player->y + y - (zoom >> 1);
        if (py < 0 || py >= map->rows) continue;
        for (int x = 0; x < zoom; x++) {
            int px = map->player->x + x - (zoom >> 1);
            if (px < 0 || px >= map->cols) continue;

            int biomeType = map->matrix[py][px].biomeType;
            Color color = CELL_COLORS[biomeType - 1];

            if (map->matrix[py][px].isWall) {
                color.r += 70;
                color.g += 70;
                color.b += 70;
            }

            DrawRectangle(x0 + x * cellSize, y0 + y * cellSize, cellSize,
                          cellSize, color);
        }
    }

    int offset = (zoom * cellSize) >> 1;
    static char key[100];
    HashTable* chunks = map->chunks;
    for (int i = -1; i < 2; i++) {
        int chunkY = map->player->chunkY + i;
        if (chunkY < 0 || chunkY >= map->chunkRows) continue;
        for (int j = -1; j < 2; j++) {
            int chunkX = map->player->chunkX + j;
            if (chunkX < 0 || chunkX >= map->chunkCols) continue;
            sprintf(key, "%d,%d", chunkY, chunkX);
            LinkedList* enemies = chunks->get(chunks, key);
            Node* cur = enemies->head;
            while (cur != NULL) {
                Enemy* e = cur->data;
                int x = (e->x - map->player->x) * cellSize;
                int y = (e->y - map->player->y) * cellSize;

                DrawRectangle(x + x0 + offset, y + y0 + offset, cellSize,
                              cellSize, (Color){255, 255, 0, 255});
                cur = cur->next;
            }

            int startChunkX =
                x0 + offset +
                (chunkX * map->chunkSize - map->player->x) * cellSize;
            int startChunkY =
                y0 + offset +
                (chunkY * map->chunkSize - map->player->y) * cellSize;
            DrawRectangleLinesEx((Rectangle){startChunkX, startChunkY,
                                             cellSize * map->chunkSize + 3,
                                             cellSize * map->chunkSize + 3},
                                 3, GREEN);
        }
    }
    DrawRectangle(x0 + offset, y0 + offset, cellSize,
                              cellSize, WHITE);
}

static void drawHudDebug(Render* this, Map* map){
    static char buffer[100];
    sprintf(buffer, "Chunk x: %d, y: %d\nCord x:%d, y:%d", map->player->chunkX, map->player->chunkY, map->player->x, map->player->y);

    DrawRectangle(this->width - 400, 0, 400,
                              400, (Color){0, 0, 0, 200});
    DrawText(buffer, this->width - 300, 50, 30, (Color){0, 255, 0, 255});

    drawMinimapDebug(this, map, 20, 20, 500, 100);
}

static void drawMapDebug(Render* this, Map* map) {
    ClearBackground(BLACK);
    int px = map->player->lastX;
    int py = map->player->lastY;
    static char buffer[100];

    int animationFrame = this->frameCount - this->lastUpdate;

    float t = animationFrame / (float)FRAMES_ANIMATION;
    if (t > 1) t = 1;

    int dx = map->player->lastX - map->player->x;
    int dy = map->player->lastY - map->player->y;

    float offsetHalfXAnimated = this->offsetHalfX;
    float offsetHalfYAnimated = this->offsetHalfY;

    if (dx) offsetHalfXAnimated += this->cellSize * t * dx;
    if (dy) offsetHalfYAnimated += this->cellSize * t * dy;

    for (int i = -this->renderDistY; i <= this->renderDistY; i++) {
        int yIdx = i + py;
        if (yIdx < 0 || yIdx >= map->rows) continue;

        for (int j = -this->renderDistX; j <= this->renderDistX; j++) {
            int xIdx = j + px;
            if (xIdx < 0 || xIdx >= map->cols) continue;

            int biomeType = map->matrix[yIdx][xIdx].biomeType;
            Color color = CELL_COLORS[biomeType - 1];

            if (map->matrix[yIdx][xIdx].isWall) {
                color.r += 70;
                color.g += 70;
                color.b += 70;
            }

            int x = offsetHalfXAnimated + j * this->cellSize;
            int y = offsetHalfYAnimated + i * this->cellSize;

            DrawRectangle(x, y, this->cellSize, this->cellSize, color);

            sprintf(buffer, "%d", map->matrix[yIdx][xIdx].distance);
            DrawText(buffer, x + 10, y + 10, 20, (Color){255, 255, 255, 255});
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
                int eDx = e->lastX - e->x;
                int eDy = e->lastY - e->y;
                float dAnimationX = eDx * t * this->cellSize;
                float dAnimationY = eDy * t * this->cellSize;
                int x = offsetHalfXAnimated + (e->lastX - px) * this->cellSize - dAnimationX;
                int y = offsetHalfYAnimated + (e->lastY - py) * this->cellSize - dAnimationY;

                DrawRectangle(x, y, this->cellSize, this->cellSize,
                              (Color){255, 255, 0, 255});
                cur = cur->next;
            }

            int startChunkX = offsetHalfXAnimated +
                              (chunkX * map->chunkSize - px) * this->cellSize;
            int startChunkY = offsetHalfYAnimated +
                              (chunkY * map->chunkSize - py) * this->cellSize;
            DrawRectangleLinesEx(
                (Rectangle){startChunkX, startChunkY,
                            this->cellSize * map->chunkSize + 5,
                            this->cellSize * map->chunkSize + 5},
                5, GREEN);
        }
    }

    // desenha o player fixo no centro
    DrawRectangle(this->offsetHalfX, this->offsetHalfY, this->cellSize,
                  this->cellSize, (Color){255, 255, 255, 255});

    drawHudDebug(this, map);
    this->frameCount++;
}

static void saveUpdate(Render* this){
    this->lastUpdate = this->frameCount;
}

static void _free(Render* this) { free(this); }

Render* new_Render(int width, int height, int cellSize) {
    Render* render = malloc(sizeof(Render));
    render->width = width;
    render->height = height;
    render->cellSize = cellSize;

    render->renderDistX = ((width / cellSize) >> 1) + 1;
    render->renderDistY = ((height / cellSize) >> 1) + 1;

    render->lastUpdate = 0;
    render->frameCount = 0;

    render->offsetHalfX = width >> 1;
    render->offsetHalfY = height >> 1;

    render->drawMapDebug = drawMapDebug;
    render->saveUpdate = saveUpdate;
    render->free = _free;
    return render;
}