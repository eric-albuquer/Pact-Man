#include "game.h"

#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "enemy.h"

static char buffer[1000];

static const Color BIOME_COLOR[4] = { { 255, 255, 0, 255 }, {0, 255, 0, 255}, {0, 0, 255, 255}, {0, 0, 255, 255} };

static void updateAnimations(Game* this) {
    for (int i = 0; i < ANIMATION_COUNT; i++) {
        UpdateAnimation(&this->animations[i]);
    }
}

static void drawCell(Game* this, Cell* cell, int x, int y, int size, bool itens) {
    if (!cell) return;

    Sprite* sprites = this->sprites;
    Animation* animations = this->animations;

    Sprite sprite = sprites[SPRITE_FLOOR_LUXURIA + cell->biome];
    Color color = WHITE;

    if (cell->type == CELL_WALL) {
        sprite = sprites[SPRITE_WALL_LUXURIA + cell->biome];
    } else if (cell->type == CELL_GRAVE) {
        sprite = sprites[SPRITE_GRAVE];
    } else if (cell->type == CELL_GRAVE_INFESTED) {
        sprite = sprites[SPRITE_GRAVE];
    } else if (cell->type == CELL_DEGENERATED_1) {
        sprite = sprites[SPRITE_DEGENERATED_1];
    } else if (cell->type == CELL_DEGENERATED_2) {
        sprite = sprites[SPRITE_DEGENERATED_2];
    } else if (cell->type == CELL_DEGENERATED_3) {
        sprite = sprites[SPRITE_DEGENERATED_3];
    } else if (cell->type == CELL_TEMPLE) {
        color = GRAY;
    }

    DrawSprite(sprite, x, y, size, color);

    color = WHITE;

    if (cell->type == CELL_FIRE_ON) {
        DrawAnimation(animations[ANIMATION_FIRE], x, y, size, color);
    } else if (cell->type == CELL_FIRE_OFF) {
        DrawAnimation(animations[ANIMATION_FIRE], x, y, size, DARKGRAY);
    } else if (isWind(cell->type)) {
        DrawAnimation(animations[ANIMATION_WIND], x, y, size, color);
    } else if (cell->type == CELL_SPIKE) {
        DrawSprite(sprites[SPRITE_SPIKE], x, y, size, color);
    } else if (cell->type == CELL_MUD) {
        DrawAnimation(animations[ANIMATION_MUD], x, y, size, color);
    } else if (cell->type == CELL_FONT_HEALTH) {
        DrawAnimation(animations[ANIMATION_FONT], x, y, size, color);
    }

    if (!itens) return;

    if (cell->type == CELL_COIN) {
        DrawAnimation(animations[ANIMATION_COIN], x, y, size, color);
    } else if (cell->type == CELL_FRAGMENT) {
        DrawAnimation(animations[ANIMATION_FRAGMENT], x, y, size, color);
    } else if (cell->type == CELL_FRUIT) {
        DrawAnimation(animations[ANIMATION_FRUIT], x, y, size, color);
    }

    sprintf(buffer, "%d", cell->distance);
    //DrawText(buffer, x + 15, y + 15, 20, WHITE);
}

static void drawEffects(Game* this, int x, int y, int size){
    Sprite *sprites = this->sprites;

    Effects effects = this->map->player->effects;

    int delta = size + 20;
    int ex = x;

    DrawRectangle(x - 20, y - 20, delta * 4 + 20, delta + 20, (Color){100, 100, 100, 100});

    if (effects.degeneration.duration > 0){
        DrawSprite(sprites[SPRITE_EFFECT_DEGENERATION], ex, y, size, WHITE);
        ex += delta;
    } 
    if (effects.regeneration.duration > 0){
        DrawSprite(sprites[SPRITE_EFFECT_REGENERATION], ex, y, size, WHITE);
        ex += delta;
    } 
    if (effects.slowness.duration > 0){
        DrawSprite(sprites[SPRITE_EFFECT_SLOWNESS], ex, y, size, WHITE);
        ex += delta;
    } 
    if (effects.invulnerability.duration > 0){
        DrawSprite(sprites[SPRITE_EFFECT_INVULNERABILITY], ex, y, size, WHITE);
    } 
}

static void drawMinimapDebug(Game* this, int x0, int y0, int size, int zoom) {
    Map* map = this->map;
    ChunkManager* cm = map->manager;
    Animation* animations = this->animations;
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

            if (!e->isBoss)
                DrawAnimation(animations[ANIMATION_PACMAN_RIGHT + e->dir], x + x0 + offset, y + y0 + offset, cellSize, BIOME_COLOR[e->biome]);
            else
                DrawAnimation(animations[ANIMATION_PACMAN_RIGHT + e->dir], x + x0 + offset - cellSize, y + y0 + offset - cellSize, cellSize * 3, BIOME_COLOR[e->biome]);

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

    int boxWidth = 220;
    int boxHeight = 80;
    int x = this->width - boxWidth - 20;
    int y = 20;

    DrawRectangle(x, y, boxWidth, boxHeight, (Color) { 0, 0, 0, 200 });

    DrawText(buffer, x + 16, y + 10, 30, WHITE);

    DrawText(stageText[stage], x + 16, y + 45, 20, stageColor);

    DrawRectangle(x + boxWidth - 30, y + 10, 16, 16, BIOME_COLOR[map->player->biome]);
}


static void drawHud(Game* this) {
    Map* map = this->map;
    Player* p = map->player;
    sprintf(buffer,
        "Life:%d\nChunk x: %d, y: %d\nCord x:%d, y:%d\ncx:%d, cy:%d\nBiome:%d\nCoins:%d\nBiome Coins:%d\nFragment:%d\nBiome Fragment:%d\nInvulnerability:%d\n",
        p->life, p->chunkX, p->chunkY, p->x, p->y, p->x & CHUNK_MASK, p->y & CHUNK_MASK,
        p->biome, p->totalCoins, p->biomeCoins, p->totalFragment, p->biomeFragment, p->effects.invulnerability.duration);

    DrawRectangle(this->width - 400, 200, 400, 600, (Color) { 0, 0, 0, 200 });
    DrawText(buffer, this->width - 300, 250, 30, GREEN);

    drawMinimapDebug(this, 20, 20, 500, 100);

    drawTimeHUD(this);
    drawEffects(this, 550, 50, 100);
}

static void drawMapDebug(Game* this) {
    Animation* animations = this->animations;
    Map* map = this->map;
    this->sounds->updateMusic(this->sounds, map->player->biome);
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

            if (!e->isBoss)
                DrawAnimation(animations[ANIMATION_PACMAN_RIGHT + e->dir], x, y, this->cellSize, BIOME_COLOR[e->biome]);
            else
                DrawAnimation(animations[ANIMATION_PACMAN_RIGHT + e->dir], x - this->cellSize, y - this->cellSize, this->cellSize * 3, BIOME_COLOR[e->biome]);

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

    DrawAnimation(animations[ANIMATION_GHOST_RIGHT + p->dir], this->offsetHalfX, this->offsetHalfY, this->cellSize, WHITE);

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
        drawHud(this);

    this->frameCount++;
}

static void loadAllSprites(Game* this) {
    Sprite* sprites = this->sprites;
    Animation* animations = this->animations;

    // ---------- GHOST ----------
    const char* ghostRight[] = { "assets/sprites/ghost/ghostRight1.png", "assets/sprites/ghost/ghostRight2.png" };
    const char* ghostLeft[] = { "assets/sprites/ghost/ghostLeft1.png",  "assets/sprites/ghost/ghostLeft2.png" };
    const char* ghostUp[] = { "assets/sprites/ghost/ghostUp1.png",    "assets/sprites/ghost/ghostUp2.png" };
    const char* ghostDown[] = { "assets/sprites/ghost/ghostDown1.png",  "assets/sprites/ghost/ghostDown2.png" };

    animations[ANIMATION_GHOST_RIGHT] = LoadAnimation(2, ghostRight);
    animations[ANIMATION_GHOST_LEFT] = LoadAnimation(2, ghostLeft);
    animations[ANIMATION_GHOST_UP] = LoadAnimation(2, ghostUp);
    animations[ANIMATION_GHOST_DOWN] = LoadAnimation(2, ghostDown);

    // ---------- PACMAN ----------
    const char* pacmanRight[] = { "assets/sprites/pacman/pacmanRight1.png", "assets/sprites/pacman/pacmanRight2.png" };
    const char* pacmanLeft[] = { "assets/sprites/pacman/pacmanLeft1.png",  "assets/sprites/pacman/pacmanLeft2.png" };
    const char* pacmanUp[] = { "assets/sprites/pacman/pacmanUp1.png",    "assets/sprites/pacman/pacmanUp2.png" };
    const char* pacmanDown[] = { "assets/sprites/pacman/pacmanDown1.png",  "assets/sprites/pacman/pacmanDown2.png" };

    animations[ANIMATION_PACMAN_RIGHT] = LoadAnimation(2, pacmanRight);
    animations[ANIMATION_PACMAN_LEFT] = LoadAnimation(2, pacmanLeft);
    animations[ANIMATION_PACMAN_UP] = LoadAnimation(2, pacmanUp);
    animations[ANIMATION_PACMAN_DOWN] = LoadAnimation(2, pacmanDown);

    const char* coin[] = { "assets/sprites/itens/coin.png" };
    const char* fragment[] = { "assets/sprites/itens/chave.png" };
    const char* fruit[] = { "assets/sprites/itens/apple.png" };

    animations[ANIMATION_COIN] = LoadAnimation(1, coin);
    animations[ANIMATION_FRAGMENT] = LoadAnimation(1, fragment);
    animations[ANIMATION_FRUIT] = LoadAnimation(1, fruit);

    const char* wind[] = { "assets/sprites/luxuria/ventania1.png", "assets/sprites/luxuria/ventania2.png" };
    const char* mud[] = { "assets/sprites/gula/lama1.png", "assets/sprites/gula/lama2.png", "assets/sprites/gula/lama3.png" };
    const char* fire[] = { "assets/sprites/heresia/fogo.png", "assets/sprites/heresia/fogo2.png", "assets/sprites/heresia/fogo3.png",
                             "assets/sprites/heresia/fogo4.png" };

    const char* font[] = { "assets/sprites/common_cells/fonte.png", "assets/sprites/common_cells/fonte1.png", "assets/sprites/common_cells/fonte2.png", "assets/sprites/common_cells/fonte3.png",
                            "assets/sprites/common_cells/fonte3.png", "assets/sprites/common_cells/fonte2.png", "assets/sprites/common_cells/fonte1.png" };

    animations[ANIMATION_WIND] = LoadAnimation(2, wind);
    animations[ANIMATION_MUD] = LoadAnimation(3, mud);
    animations[ANIMATION_FIRE] = LoadAnimation(4, fire);

    animations[ANIMATION_FONT] = LoadAnimation(7, font);

    sprites[SPRITE_FLOOR_LUXURIA] = LoadSprite("assets/sprites/luxuria/chao.png");
    sprites[SPRITE_WALL_LUXURIA] = LoadSprite("assets/sprites/luxuria/parede.png");

    sprites[SPRITE_FLOOR_GULA] = LoadSprite("assets/sprites/gula/chao.png");
    sprites[SPRITE_WALL_GULA] = LoadSprite("assets/sprites/gula/parede.png");

    sprites[SPRITE_FLOOR_HERESIA] = LoadSprite("assets/sprites/heresia/chao.png");
    sprites[SPRITE_WALL_HERESIA] = LoadSprite("assets/sprites/heresia/parede.png");

    sprites[SPRITE_FLOOR_VIOLENCIA] = LoadSprite("assets/sprites/violencia/chao.png");
    sprites[SPRITE_WALL_VIOLENCIA] = LoadSprite("assets/sprites/violencia/parede.png");

    sprites[SPRITE_DEGENERATED_1] = LoadSprite("assets/sprites/common_cells/degenerated1.png");
    sprites[SPRITE_DEGENERATED_2] = LoadSprite("assets/sprites/common_cells/degenerated2.png");
    sprites[SPRITE_DEGENERATED_3] = LoadSprite("assets/sprites/common_cells/degenerated3.png");

    sprites[SPRITE_GRAVE] = LoadSprite("assets/sprites/heresia/cova.png");
    sprites[SPRITE_SPIKE] = LoadSprite("assets/sprites/violencia/espinhos.png");

    sprites[SPRITE_EFFECT_REGENERATION] = LoadSprite("assets/sprites/effects/regeneration.png");
    sprites[SPRITE_EFFECT_DEGENERATION] = LoadSprite("assets/sprites/effects/degeneration.png");
    sprites[SPRITE_EFFECT_INVULNERABILITY] = LoadSprite("assets/sprites/effects/invulnerability.png");
    sprites[SPRITE_EFFECT_SLOWNESS] = LoadSprite("assets/sprites/effects/slowness.png");
}

static void saveUpdate(Game* this) {
    updateAnimations(this);
    this->lastUpdate = this->frameCount;
    this->updateCount++;
}

static void _free(Game* this) {
    for (int i = 0; i < ANIMATION_COUNT; i++) {
        UnloadAnimation(this->animations[i]);
    }

    for (int i = 0; i < SPRITE_COUNT; i++) {
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
