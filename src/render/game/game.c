#include "game.h"

#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "enemy.h"

static const Color CELL_COLORS[4] = { {30, 30, 30, 255}, {160, 0, 0, 255}, {0, 100, 0, 255}, {0, 0, 150, 255} };

static char buffer[1000];

static void updateAnimations(Game* this){
    for (int i = 0; i < ANIMATION_COUNT; i++){
        UpdateAnimation(&this->animations[i]);
    }
}

static void drawCell(Game* this, Cell* cell, int x, int y, int size, bool itens) {
    if (!cell) return;
    Color color = CELL_COLORS[cell->biome];

    Sprite* sprites = this->sprites;
    Animation* animations = this->animations;

    DrawSprite(sprites[SPRITE_FLOOR_LUXURIA + cell->biome], x, y, size);

    if (cell->type == CELL_WALL) {
        DrawSprite(sprites[SPRITE_WALL_LUXURIA + cell->biome], x, y, size);
        color.r += 70;
        color.g += 70;
        color.b += 70;
    } else  if (isWind(cell->type)) {
        color.r *= 0.7;
        color.g += 100;
    } else if (cell->type == CELL_MUD) {
        DrawSprite(sprites[SPRITE_MUD], x, y, size);
        color.r = 79;
        color.g = 39;
        color.b = 30;
    } else if (cell->type == CELL_SPIKE) {
        color.r = 185;
        color.g = 185;
        color.b = 185;
    } else if (cell->type == CELL_GRAVE) {
        DrawSprite(sprites[SPRITE_GRAVE], x, y, size);
        color.r = 10;
        color.g = 10;
        color.b = 10;
    } else if (cell->type == CELL_GRAVE_INFESTED) {
        DrawSprite(sprites[SPRITE_GRAVE], x, y, size);
        color.r = 10;
        color.g = 10;
        color.b = 100;
    } else if (cell->type == CELL_FIRE_ON) {
        color.r = 255;
        color.g = 0;
        color.b = 0;
    } else if (cell->type == CELL_FIRE_OFF) {
        color.r = 128;
        color.g = 128;
        color.b = 128;
    } else if (cell->type == CELL_FONT_HEALTH) {
        color.r = 0;
        color.g = 100;
        color.b = 200;
    } else if (cell->type == CELL_TEMPLE) {
        color.r -= 20;
        color.g -= 20;
        color.b -= 20;
    } else if (cell->type == CELL_DEGENERATED) {
        color = BLANK;
    }

    if (!itens) {
        DrawRectangle(x, y, size, size, color);
        return;
    }

    if (cell->type == CELL_COIN) {
        DrawAnimation(animations[ANIMATION_COIN], x, y, size);
    } else if (cell->type == CELL_FRAGMENT) {
        DrawAnimation(animations[ANIMATION_FRAGMENT], x, y, size);
    } else if (cell->type == CELL_FRUIT) {
        DrawAnimation(animations[ANIMATION_FRUIT], x, y, size);
    } else if (cell->type == CELL_FIRE_ON) {
        DrawAnimation(animations[ANIMATION_FIRE], x, y, size);
    } else if (isWind(cell->type)) {
        DrawAnimation(animations[ANIMATION_WIND], x, y, size);
    } else if (cell->type == CELL_SPIKE) {
        DrawSprite(sprites[SPRITE_SPIKE], x, y, size);
    }

    sprintf(buffer, "%d", cell->distance);
    DrawText(buffer, x + 15, y + 15, 20, WHITE);
}

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
            drawCell(this, cell, x0 + x * cellSize, y0 + y * cellSize, cellSize, false);
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

            DrawRectangle(x + x0 + offset, y + y0 + offset, cellSize, cellSize, YELLOW);
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
            DrawRectangleLinesEx((Rectangle) { startChunkX, startChunkY, cellSize* CHUNK_SIZE + 3, cellSize* CHUNK_SIZE + 3 }, 3, GREEN);
        }
    }

    DrawRectangle(x0 + offset, y0 + offset, cellSize, cellSize, WHITE);
}

static void drawTimeHUD(Game* this) {
    Map* map = this->map;
    Player* p = map->player;

    int totalSeconds = (int)map->elapsedTime;
    int mm = totalSeconds / 60;
    int ss = totalSeconds % 60;

    sprintf(buffer, "%02d:%02d", mm, ss);

    float d = map->degenerescence;
    int stage = 0;
    if (d > 0.0f && d < 0.34f) stage = 1;
    else if (d >= 0.34f && d < 0.67f) stage = 2;
    else if (d >= 0.67f) stage = 3;

    const char* stageText[] = {
        "STABLE",
        "UNSTABLE",
        "CRACKING",
        "COLLAPSING"
    };

    Color stageColors[] = {
        { 0,   228, 48,  255 },
        { 253, 249, 0,   255 },
        { 255, 161, 0,   255 },
        { 230, 41,  55,  255 }
    };

    Color stageColor = stageColors[stage];

    Color biomeColor = CELL_COLORS[p->biome];
    biomeColor.a = 255;

    int boxWidth = 220;
    int boxHeight = 80;
    int x = this->width - boxWidth - 20;
    int y = 20;

    DrawRectangle(x, y, boxWidth, boxHeight, (Color) { 0, 0, 0, 200 });

    DrawText(buffer, x + 16, y + 10, 30, WHITE);

    DrawText(stageText[stage], x + 16, y + 45, 20, stageColor);

    DrawRectangle(x + boxWidth - 30, y + 10, 16, 16, biomeColor);
}


static void drawHudDebug(Game* this) {
    Map* map = this->map;
    Player* p = map->player;
    sprintf(buffer,
        "Life:%d\nChunk x: %d, y: %d\nCord x:%d, y:%d\ncx:%d, cy:%d\nBiome:%d\nCoins:%d\nBiome Coins:%d\nFragment:%d\nBiome Fragment:%d\nInvulnerability:%d\n",
        p->life, p->chunkX, p->chunkY, p->x, p->y, p->x & CHUNK_MASK, p->y & CHUNK_MASK,
        p->biome, p->totalCoins, p->biomeCoins, p->totalFragment, p->biomeFragment, p->effects.invulnerability.duration);

    DrawRectangle(this->width - 400, 0, 400, 400, (Color) { 0, 0, 0, 200 });
    DrawText(buffer, this->width - 300, 50, 30, GREEN);

    drawMinimapDebug(this, 20, 20, 500, 100);

    drawTimeHUD(this);
}

static void drawMapDebug(Game* this) {
    Animation* animations = this->animations;
    Map* map = this->map;
    //this->sounds->updateMusic(this->sounds, map->player->biome);
    ChunkManager* cm = map->manager;
    ClearBackground(BLACK);
    Player* p = map->player;
    int px = p->lastX;
    int py = p->lastY;

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

            int x = offsetHalfXAnimated + j * this->cellSize;
            int y = offsetHalfYAnimated + i * this->cellSize;

            drawCell(this, cell, x, y, this->cellSize, true);
        }
    }

    if (p->biome == 3) {
        BeginTextureMode(this->shadowMap);
        ClearBackground(BLACK);
        DrawCircleGradient(this->offsetHalfX, this->offsetHalfY, 400, WHITE, BLANK);
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
            int x = offsetHalfXAnimated + (e->lastX - px) * this->cellSize - dAnimationX;
            int y = offsetHalfYAnimated + (e->lastY - py) * this->cellSize - dAnimationY;

            if (p->biome == 3) {
                BeginTextureMode(this->shadowMap);
                DrawCircleGradient(x, y, 150, WHITE, BLANK);
                EndTextureMode();
            }

            DrawAnimation(animations[ANIMATION_PACMAN_RIGHT + e->dir], x, y, this->cellSize);

            cur = cur->next;
        }
    }

    if (p->biome == 3) {
        BeginBlendMode(BLEND_MULTIPLIED);
        DrawTextureRec(this->shadowMap.texture,
            (Rectangle) {
            0, 0, this->shadowMap.texture.width, -this->shadowMap.texture.height
        },
            (Vector2) {
            0, 0
        }, WHITE);
        EndBlendMode();
    }

    DrawAnimation(animations[ANIMATION_GHOST_RIGHT + p->dir], this->offsetHalfX, this->offsetHalfY, this->cellSize);

    for (int i = -1; i < 2; i++) {
        int chunkY = map->player->chunkY + i;
        if (chunkY < 0 || chunkY >= map->manager->rows) continue;
        for (int j = -1; j < 2; j++) {
            int chunkX = map->player->chunkX + j;
            if (chunkX < 0 || chunkX >= map->manager->cols) continue;

            int startChunkX = offsetHalfXAnimated + (chunkX * CHUNK_SIZE - px) * this->cellSize;
            int startChunkY = offsetHalfYAnimated + (chunkY * CHUNK_SIZE - py) * this->cellSize;
            DrawRectangleLinesEx((Rectangle) { startChunkX, startChunkY, this->cellSize* CHUNK_SIZE + 5, this->cellSize* CHUNK_SIZE + 5 }, 5, GREEN);
        }
    }

    if (p->biome != 3)
        drawHudDebug(this);

    this->frameCount++;
}

static void loadAllSprites(Game* this) {
    Sprite* sprites = this->sprites;
    Animation* animations = this->animations;

    // ---------- GHOST ----------
    const char* ghostRight[] = { "assets/sprites/ghostRight1.png", "assets/sprites/ghostRight2.png" };
    const char* ghostLeft[] = { "assets/sprites/ghostLeft1.png",  "assets/sprites/ghostLeft2.png" };
    const char* ghostUp[] = { "assets/sprites/ghostUp1.png",    "assets/sprites/ghostUp2.png" };
    const char* ghostDown[] = { "assets/sprites/ghostDown1.png",  "assets/sprites/ghostDown2.png" };

    animations[ANIMATION_GHOST_RIGHT] = LoadAnimation(2, ghostRight);
    animations[ANIMATION_GHOST_LEFT] = LoadAnimation(2, ghostLeft);
    animations[ANIMATION_GHOST_UP] = LoadAnimation(2, ghostUp);
    animations[ANIMATION_GHOST_DOWN] = LoadAnimation(2, ghostDown);

    // ---------- PACMAN ----------
    const char* pacmanRight[] = { "assets/sprites/pacmanRight1.png", "assets/sprites/pacmanRight2.png" };
    const char* pacmanLeft[] = { "assets/sprites/pacmanLeft1.png",  "assets/sprites/pacmanLeft2.png" };
    const char* pacmanUp[] = { "assets/sprites/pacmanUp1.png",    "assets/sprites/pacmanUp2.png" };
    const char* pacmanDown[] = { "assets/sprites/pacmanDown1.png",  "assets/sprites/pacmanDown2.png" };

    animations[ANIMATION_PACMAN_RIGHT] = LoadAnimation(2, pacmanRight);
    animations[ANIMATION_PACMAN_LEFT] = LoadAnimation(2, pacmanLeft);
    animations[ANIMATION_PACMAN_UP] = LoadAnimation(2, pacmanUp);
    animations[ANIMATION_PACMAN_DOWN] = LoadAnimation(2, pacmanDown);

    const char* coin[] = { "assets/sprites/coin.png" };
    const char* fragment[] = { "assets/sprites/Key.png" };
    const char* fruit[] = { "assets/sprites/apple.png" };

    animations[ANIMATION_COIN] = LoadAnimation(1, coin);
    animations[ANIMATION_FRAGMENT] = LoadAnimation(1, fragment);
    animations[ANIMATION_FRUIT] = LoadAnimation(1, fruit);

    const char* wind[] = { "assets/sprites/luxuria/ventania1.png", "assets/sprites/luxuria/ventania2.png" };
    const char* fire[] = { "assets/sprites/heresia/fogo.png" };

    animations[ANIMATION_WIND] = LoadAnimation(2, wind);
    animations[ANIMATION_FIRE] = LoadAnimation(2, fire);
    
    sprites[SPRITE_FLOOR_LUXURIA] = LoadSprite("assets/sprites/luxuria/chao.png");
    sprites[SPRITE_WALL_LUXURIA] = LoadSprite("assets/sprites/luxuria/parede.png");

    sprites[SPRITE_FLOOR_GULA] = LoadSprite("assets/sprites/gula/chao.png");
    sprites[SPRITE_WALL_GULA] = LoadSprite("assets/sprites/gula/parede.png");

    sprites[SPRITE_FLOOR_HERESIA] = LoadSprite("assets/sprites/heresia/chao.png");
    sprites[SPRITE_WALL_HERESIA] = LoadSprite("assets/sprites/heresia/parede.png");

    sprites[SPRITE_FLOOR_VIOLENCIA] = LoadSprite("assets/sprites/violencia/chao.png");
    sprites[SPRITE_WALL_VIOLENCIA] = LoadSprite("assets/sprites/violencia/parede.png");

    sprites[SPRITE_MUD] = LoadSprite("assets/sprites/gula/lama.png");
    sprites[SPRITE_GRAVE] = LoadSprite("assets/sprites/heresia/cova.png");
    sprites[SPRITE_SPIKE] = LoadSprite("assets/sprites/violencia/espinhos.png");
}

static void saveUpdate(Game* this) {
    updateAnimations(this);
    this->lastUpdate = this->frameCount;
    this->updateCount++;
}

static void _free(Game* this) {
    for (int i = 0; i < ANIMATION_COUNT; i++){
        UnloadAnimation(this->animations[i]);
    }

    for (int i = 0; i < SPRITE_COUNT; i++){
        UnloadSprite(this->sprites[i]);
    }
    
    UnloadRenderTexture(this->shadowMap);
    free(this);
}

Game* new_Game(int width, int height, int cellSize, Map* map) {
    Game* this = malloc(sizeof(Game));
    this->width = width;
    this->height = height;
    this->cellSize = cellSize;

    this->sounds = new_Sounds("assets/music/heresia_trilha.mp3", "assets/music/heresia_trilha.mp3", "assets/music/violencia_trilha.mp3", "assets/music/luxuria_trilha.mp3");

    this->renderDistX = ((width / cellSize) >> 1) + 3;
    this->renderDistY = ((height / cellSize) >> 1) + 3;

    this->lastUpdate = 0;
    this->frameCount = 0;

    this->offsetHalfX = width >> 1;
    this->offsetHalfY = height >> 1;

    this->map = map;

    loadAllSprites(this);

    this->shadowMap = LoadRenderTexture(this->width, this->height);

    this->drawMapDebug = drawMapDebug;
    this->saveUpdate = saveUpdate;
    this->free = _free;
    return this;
}
