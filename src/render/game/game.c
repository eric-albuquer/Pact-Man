#include "game.h"

#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "enemy.h"

static const char* SPRITES[] = {
    "assets/sprites/ghostRight1.png",  "assets/sprites/ghostRight2.png",
    "assets/sprites/ghostDown1.png",   "assets/sprites/ghostDown2.png",
    "assets/sprites/ghostLeft1.png",   "assets/sprites/ghostLeft2.png",
    "assets/sprites/ghostUp1.png",     "assets/sprites/ghostUp2.png",

    "assets/sprites/pacmanRight1.png", "assets/sprites/pacmanRight2.png",
    "assets/sprites/pacmanDown1.png",  "assets/sprites/pacmanDown2.png",
    "assets/sprites/pacmanLeft1.png",  "assets/sprites/pacmanLeft2.png",
    "assets/sprites/pacmanUp1.png",    "assets/sprites/pacmanUp2.png",

    "assets/sprites/coin.png",         "assets/sprites/Key.png",
    "assets/sprites/apple.png"};

static const Color CELL_COLORS[4] = {
    {30, 30, 30, 255}, {160, 0, 0, 255}, {0, 100, 0, 255}, {0, 0, 150, 255}};

static void drawMinimapDebug(Game* this, int x0, int y0, int size, int zoom) {
    Map* map = this->map;
    ChunkManager* cm = map->manager;
    int cellSize = size / (zoom);
    DrawRectangle(x0 - 5, y0 - 5, size + 10, size + 10, BLACK);
    for (int y = 0; y < zoom; y++) {
        int py = map->player->y + y - (zoom >> 1);
        for (int x = 0; x < zoom; x++) {
            int px = map->player->x + x - (zoom >> 1);

            Cell* cell = cm->getLoadedCell(cm, px, py);
            if (!cell) continue;
            int biomeType = cell->biome;
            Color color = CELL_COLORS[biomeType];

            if (cell->type == CELL_WALL) {
                color.r += 70;
                color.g += 70;
                color.b += 70;
            }

            else if (isWind(cell->type)) {
                color.r *= 0.7;
                color.g += 100;
            }

            else if (cell->type == CELL_MUD) {
                color.r = 79;
                color.g = 39;
                color.b = 30;
            }

            else if (cell->type == CELL_SPIKE) {
                color.r = 185;
                color.g = 185;
                color.b = 185;
            }

            else if (cell->type == CELL_GRAVE) {
                color.r = 10;
                color.g = 10;
                color.b = 10;
            }

            else if (cell->type == CELL_FIRE) {
                color.r = 255;
                color.g = 0;
                color.b = 0;
            }

            DrawRectangle(x0 + x * cellSize, y0 + y * cellSize, cellSize,
                          cellSize, color);
        }
    }

    int offset = (zoom * cellSize) >> 1;

    for (int i = 0; i < 9; i++) {
        int idx = CLOSER_IDX[i];
        Chunk* chunk = cm->adjacents[idx];
        if (!chunk) continue;
        Node* cur = chunk->enemies->head;
        while (cur != NULL) {
            Enemy* e = cur->data;
            int x = (e->x - map->player->x) * cellSize;
            int y = (e->y - map->player->y) * cellSize;

            DrawRectangle(x + x0 + offset, y + y0 + offset, cellSize, cellSize,
                          (Color){255, 255, 0, 255});
            cur = cur->next;
        }
    }

    for (int i = -1; i < 2; i++) {
        int chunkY = map->player->chunkY + i;
        if (chunkY < 0 || chunkY >= map->manager->rows) continue;
        for (int j = -1; j < 2; j++) {
            int chunkX = map->player->chunkX + j;
            if (chunkX < 0 || chunkX >= map->manager->cols) continue;
            int startChunkX =
                x0 + offset + (chunkX * CHUNK_SIZE - map->player->x) * cellSize;
            int startChunkY =
                y0 + offset + (chunkY * CHUNK_SIZE - map->player->y) * cellSize;
            DrawRectangleLinesEx(
                (Rectangle){startChunkX, startChunkY, cellSize * CHUNK_SIZE + 3,
                            cellSize * CHUNK_SIZE + 3},
                3, GREEN);
        }
    }

    DrawRectangle(x0 + offset, y0 + offset, cellSize, cellSize, WHITE);
}

static void drawHudDebug(Game* this) {
    Map* map = this->map;
    static char buffer[1000];
    Player* p = map->player;
    sprintf(buffer,
            "Chunk x: %d, y: %d\nCord x:%d, y:%d\ncx:%d, cy:%d\nBiome:%d",
            p->chunkX, p->chunkY, p->x, p->y, p->x & CHUNK_MASK,
            p->y & CHUNK_MASK, p->biome);

    DrawRectangle(this->width - 400, 0, 400, 400, (Color){0, 0, 0, 200});
    DrawText(buffer, this->width - 300, 50, 30, (Color){0, 255, 0, 255});

    drawMinimapDebug(this, 20, 20, 500, 100);
}

static void drawMapDebug(Game* this) {
    Map* map = this->map;
    ChunkManager* cm = map->manager;
    ClearBackground(BLACK);
    Player* p = map->player;
    int px = p->lastX;
    int py = p->lastY;
    static char buffer[100];

    int animationFrame = this->frameCount - this->lastUpdate;

    float t = animationFrame / FRAMES_ANIMATION;
    if (t > 1) t = 1;

    int dx = p->lastX - p->x;
    int dy = p->lastY - p->y;

    float offsetHalfXAnimated = this->offsetHalfX;
    float offsetHalfYAnimated = this->offsetHalfY;

    if (dx) offsetHalfXAnimated += this->cellSize * t * dx;
    if (dy) offsetHalfYAnimated += this->cellSize * t * dy;

    for (int i = -this->renderDistY; i <= this->renderDistY; i++) {
        int yIdx = i + py;

        for (int j = -this->renderDistX; j <= this->renderDistX; j++) {
            int xIdx = j + px;

            Cell* cell = cm->getLoadedCell(cm, xIdx, yIdx);
            if (!cell) continue;
            int biomeType = cell->biome;
            Color color = CELL_COLORS[biomeType];

            if (cell->type == CELL_WALL) {
                color.r += 70;
                color.g += 70;
                color.b += 70;
            }

            else if (isWind(cell->type)) {
                color.r *= 0.7;
                color.g += 100;
            }

            else if (cell->type == CELL_MUD) {
                color.r = 79;
                color.g = 39;
                color.b = 30;
            }

            else if (cell->type == CELL_SPIKE) {
                color.r = 185;
                color.g = 185;
                color.b = 185;
            }

            else if (cell->type == CELL_GRAVE) {
                color.r = 10;
                color.g = 10;
                color.b = 10;
            }

            else if (cell->type == CELL_FIRE) {
                color.r = 255;
                color.g = 0;
                color.b = 0;
            }

            int x = offsetHalfXAnimated + j * this->cellSize;
            int y = offsetHalfYAnimated + i * this->cellSize;

            DrawRectangle(x, y, this->cellSize, this->cellSize, color);

            if (cell->type == CELL_COIN) {
                Texture2D enemyTexture = this->sprites[16];
                DrawTexture(enemyTexture, x, y, (Color){255, 255, 255, 255});
            }

            else if (cell->type == CELL_FRAGMENT) {
                Texture2D enemyTexture = this->sprites[17];
                DrawTexture(enemyTexture, x, y, (Color){255, 255, 255, 255});
            }

            else if (cell->type == CELL_FRUIT) {
                Texture2D enemyTexture = this->sprites[18];
                DrawTexture(enemyTexture, x, y, (Color){255, 255, 255, 255});
            }

            sprintf(buffer, "%d", cell->distance);
            DrawText(buffer, x + 15, y + 15, 20, (Color){255, 255, 255, 255});
        }
    }

    if (p->biome == 3) {
        BeginTextureMode(this->shadowMap);
        ClearBackground(BLACK);
        DrawCircleGradient(this->offsetHalfX, this->offsetHalfY, 400, WHITE,
                           BLANK);
        EndTextureMode();
    }

    for (int i = 0; i < 9; i++) {
        int idx = CLOSER_IDX[i];
        Chunk* chunk = cm->adjacents[idx];
        if (!chunk) continue;
        Node* cur = chunk->enemies->head;
        while (cur != NULL) {
            Enemy* e = cur->data;
            int eDx = e->lastX - e->x;
            int eDy = e->lastY - e->y;
            float dAnimationX = eDx * t * this->cellSize;
            float dAnimationY = eDy * t * this->cellSize;
            int x = offsetHalfXAnimated + (e->lastX - px) * this->cellSize -
                    dAnimationX;
            int y = offsetHalfYAnimated + (e->lastY - py) * this->cellSize -
                    dAnimationY;

            int offsetTexture = 8;
            if (e->dir == RIGHT)
                offsetTexture = 8;
            else if (e->dir == DOWN)
                offsetTexture = 10;
            else if (e->dir == LEFT)
                offsetTexture = 12;
            else if (e->dir == UP)
                offsetTexture = 14;

            if (p->biome == 3) {
                BeginTextureMode(this->shadowMap);
                DrawCircleGradient(x, y, 150, WHITE, BLANK);
                EndTextureMode();
            }

            Texture2D enemyTexture =
                this->sprites[(this->updateCount % 2) + offsetTexture];
            DrawTexture(enemyTexture, x, y, (Color){255, 255, 255, 255});

            cur = cur->next;
        }
    }

    if (p->biome == 3) {
        BeginBlendMode(BLEND_MULTIPLIED);
        DrawTextureRec(this->shadowMap.texture,
                       (Rectangle){0, 0, this->shadowMap.texture.width,
                                   -this->shadowMap.texture.height},
                       (Vector2){0, 0}, WHITE);
        EndBlendMode();
    }

    int offsetTexture = 0;
    if (p->dir == RIGHT)
        offsetTexture = 0;
    else if (p->dir == DOWN)
        offsetTexture = 2;
    else if (p->dir == LEFT)
        offsetTexture = 4;
    else if (p->dir == UP)
        offsetTexture = 6;

    Texture2D playerTexture =
        this->sprites[(this->updateCount % 2) + offsetTexture];
    DrawTexture(playerTexture, this->offsetHalfX, this->offsetHalfY,
                (Color){255, 255, 255, 255});

    for (int i = -1; i < 2; i++) {
        int chunkY = map->player->chunkY + i;
        if (chunkY < 0 || chunkY >= map->manager->rows) continue;
        for (int j = -1; j < 2; j++) {
            int chunkX = map->player->chunkX + j;
            if (chunkX < 0 || chunkX >= map->manager->cols) continue;

            int startChunkX = offsetHalfXAnimated +
                              (chunkX * CHUNK_SIZE - px) * this->cellSize;
            int startChunkY = offsetHalfYAnimated +
                              (chunkY * CHUNK_SIZE - py) * this->cellSize;
            DrawRectangleLinesEx((Rectangle){startChunkX, startChunkY,
                                             this->cellSize * CHUNK_SIZE + 5,
                                             this->cellSize * CHUNK_SIZE + 5},
                                 5, GREEN);
        }
    }

    drawHudDebug(this);

    this->frameCount++;
}

static void loadSprites(Game* this, const char** paths, int total) {
    this->sprites = malloc(sizeof(Texture2D) * total);
    this->totalSprites = total;

    for (int i = 0; i < total; i++) {
        Image img = LoadImage(paths[i]);
        ImageResize(&img, this->cellSize, this->cellSize);
        Texture2D spriteTexture = LoadTextureFromImage(img);
        this->sprites[i] = spriteTexture;
        UnloadImage(img);
    }
}

static void saveUpdate(Game* this) {
    if (this->map->changedChunk) {
        this->map->manager->changedChunk = false;
    }

    this->lastUpdate = this->frameCount;
    this->updateCount++;
}

static void _free(Game* this) {
    Texture2D* sprites = this->sprites;
    for (unsigned int i = 0; i < this->totalSprites; i++) {
        Texture2D spriteTexture = sprites[i];
        UnloadTexture(spriteTexture);
    }
    UnloadRenderTexture(this->shadowMap);
    free(this->sprites);
    free(this);
}

Game* new_Game(int width, int height, int cellSize, Map* map) {
    Game* this = malloc(sizeof(Game));
    this->width = width;
    this->height = height;
    this->cellSize = cellSize;

    this->renderDistX = ((width / cellSize) >> 1) + 3;
    this->renderDistY = ((height / cellSize) >> 1) + 3;

    this->lastUpdate = 0;
    this->frameCount = 0;

    this->offsetHalfX = width >> 1;
    this->offsetHalfY = height >> 1;

    this->map = map;

    loadSprites(this, SPRITES, 19);
    this->shadowMap = LoadRenderTexture(this->width, this->height);

    this->drawMapDebug = drawMapDebug;
    this->saveUpdate = saveUpdate;
    this->free = _free;
    return this;
}