#include "game.h"

#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "enemy.h"

typedef enum {
    SPRITE_WALL_LUXURIA,
    SPRITE_WALL_GULA,
    SPRITE_WALL_HERESIA,
    SPRITE_WALL_VIOLENCIA,

    SPRITE_FLOOR_LUXURIA,
    SPRITE_FLOOR_GULA,
    SPRITE_FLOOR_HERESIA,
    SPRITE_FLOOR_VIOLENCIA,

    SPRITE_DEGENERATED_1,
    SPRITE_DEGENERATED_2,
    SPRITE_DEGENERATED_3,

    SPRITE_HEAVEN,

    SPRITE_GRAVE,

    SPRITE_ICE,

    SPRITE_EFFECT_REGENERATION,
    SPRITE_EFFECT_SLOWNESS,
    SPRITE_EFFECT_INVULNERABILITY,
    SPRITE_EFFECT_DEGENERATION,
    SPRITE_EFFECT_INVISIBILITY,
    SPRITE_EFFECT_FREEZE_TIME,

    SPRITE_MINIMAP,
    SPRITE_LIFE_BAR,
    SPRITE_BATERY_BAR,
    SPRITE_ARROW_NEXT_BIOME,

    SPRITE_GAME_OVER,

    SPRITE_COUNT
} SpritesEnum;

typedef enum {
    ANIMATION_GHOST_RIGHT,
    ANIMATION_GHOST_LEFT,
    ANIMATION_GHOST_UP,
    ANIMATION_GHOST_DOWN,

    ANIMATION_PACMAN_RIGHT,
    ANIMATION_PACMAN_LEFT,
    ANIMATION_PACMAN_UP,
    ANIMATION_PACMAN_DOWN,

    ANIMATION_COIN,
    ANIMATION_FRAGMENT,

    ANIMATION_BATERY,

    ANIMATION_PORTAL,

    ANIMATION_HORIZONTAL_WIND,
    ANIMATION_VERTICAL_WIND,
    ANIMATION_MUD,
    ANIMATION_FIRE,
    ANIMATION_TENTACLE,
    ANIMATION_SPIKE,

    ANIMATION_FONT,

    ANIMATION_COUNT
} AnimationsEnum;

typedef enum {
    MUSIC_LUXURIA,
    MUSIC_GULA,
    MUSIC_HERESIA,
    MUSIC_VIOLENCIA,

    MUSIC_INVULNERABILITY,
    MUSIC_INVISIBILITY,
    MUSIC_FREEZE_TIME,

    MUSIC_DEGENERATION,
    MUSIC_HEAVEN,

    MUSIC_GAME_OVER,

    MUSIC_COUNT
} MusicEnum;

typedef enum {
    SOUND_COIN,
    SOUND_WIND,
    SOUND_DAMAGE,
    SOUND_KILL_ENEMY,
    SOUND_FRAGMENT,
    SOUND_FRUIT,
    SOUND_SPIKE,
    SOUND_FREEZE_TIME,
    SOUND_REGENERATE,
    SOUND_POTION,
    SOUND_BATERY,
    SOUND_MUD,
    SOUND_BIOME_FREE,
    SOUND_TENTACLE,
    SOUND_FIRE,

    SOUND_DEATH,
    SOUND_CLICK_BUTTON,

    SOUND_COUNT
} SoundsEnum;

static char buffer[1000];

static const Color BIOME_COLOR[4] = { { 255, 255, 0, 255 }, {100, 0, 255, 255}, {0, 255, 0, 255}, {0, 255, 255, 255} };
static const Color HUD_OPACITY = { 0, 0, 0, 200 };

static inline Color getNegativeColor(Color color) {
    return (Color) { (color.r + 128) & 255, (color.g + 128) & 255, (color.b + 128) & 255, 255 };
}

static void updateAnimations(Game* this) {
    for (int i = 0; i < ANIMATION_COUNT; i++) {
        UpdateAnimation(&this->animations[i]);
    }
}

static void drawCell(Game* this, Cell* cell, int x, int y, int size, bool itens) {
    if (!cell) return;

    Sprite* sprites = this->sprites;
    Animation* animations = this->animations;
    Player* p = this->map->player;

    Sprite sprite = sprites[SPRITE_FLOOR_LUXURIA + cell->biome];
    Color color = WHITE;

    if (cell->type == CELL_WALL) {
        sprite = sprites[SPRITE_WALL_LUXURIA + cell->biome];
    } else if (cell->type == CELL_DEGENERATED_1) {
        sprite = sprites[SPRITE_DEGENERATED_1];
    } else if (cell->type == CELL_DEGENERATED_2) {
        sprite = sprites[SPRITE_DEGENERATED_2];
    } else if (cell->type == CELL_DEGENERATED_3) {
        sprite = sprites[SPRITE_DEGENERATED_3];
    } else if (cell->type == CELL_TEMPLE) {
        color = GRAY;
    } if (cell->type == CELL_HEAVEN) {
        sprite = sprites[SPRITE_HEAVEN];
    }

    DrawSprite(sprite, x, y, size, size, color);

    color = WHITE;

    if (cell->type == CELL_FIRE_ON) {
        DrawAnimation(animations[ANIMATION_FIRE], x, y, size, color);
    } else if (cell->type == CELL_FIRE_OFF) {
        DrawAnimation(animations[ANIMATION_FIRE], x, y, size, DARKGRAY);
    } else if (cell->type == CELL_WIND_RIGHT || cell->type == CELL_WIND_LEFT) {
        DrawAnimation(animations[ANIMATION_HORIZONTAL_WIND], x, y, size, color);
    } else if (cell->type == CELL_WIND_UP || cell->type == CELL_WIND_DOWN) {
        DrawAnimation(animations[ANIMATION_VERTICAL_WIND], x, y, size, color);
    } else if (cell->type == CELL_SPIKE) {
        DrawAnimation(animations[ANIMATION_SPIKE], x, y, size, color);
    } else if (cell->type == CELL_MUD) {
        DrawAnimation(animations[ANIMATION_MUD], x, y, size, color);
    } else if (cell->type == CELL_FONT_HEALTH) {
        if (p->cellType == CELL_FONT_HEALTH)
            DrawAnimationFrame(animations[ANIMATION_FONT], x, y, size, color, 3);
        else
            DrawAnimation(animations[ANIMATION_FONT], x, y, size, color);
    } else if (cell->type == CELL_GRAVE) {
        DrawSprite(sprites[SPRITE_GRAVE], x, y, size, size, color);
    } else if (cell->type == CELL_GRAVE_INFESTED) {
        DrawSprite(sprites[SPRITE_GRAVE], x, y, size, size, (Color) { 0, 255, 255, 255 });
    } else if (cell->type == CELL_PORTAL) {
        DrawAnimation(animations[ANIMATION_PORTAL], x, y, size, color);
    }

    if (!itens) return;


    if (cell->type == CELL_COIN) {
        static const float coinSize = 0.7f;
        float w = (1 - coinSize) * size;
        float hw = w * 0.5;
        DrawAnimation(animations[ANIMATION_COIN], x + hw, y + hw, size * coinSize, color);
    } else if (cell->type == CELL_FRAGMENT) {
        DrawAnimation(animations[ANIMATION_FRAGMENT], x, y, size, color);
    } else if (cell->type == CELL_FRUIT) {
        DrawSprite(sprites[SPRITE_EFFECT_INVULNERABILITY], x, y, size, size, color);
        if (p->effects.freezeTime.duration > 0)
            DrawSprite(sprites[SPRITE_ICE], x, y, size, size, (Color) { 255, 255, 255, 150 });
    } else if (cell->type == CELL_INVISIBILITY) {
        DrawSprite(sprites[SPRITE_EFFECT_INVISIBILITY], x, y, size, size, color);
    } else if (cell->type == CELL_REGENERATION) {
        DrawSprite(sprites[SPRITE_EFFECT_REGENERATION], x, y, size, size, color);
    } else if (cell->type == CELL_TENTACLE) {
        DrawAnimation(animations[ANIMATION_TENTACLE], x, y, size, color);
    } else if (cell->type == CELL_BATERY) {
        DrawAnimation(animations[ANIMATION_BATERY], x, y, size, color);
    } else if (cell->type == CELL_FREEZE_TIME) {
        DrawSprite(sprites[SPRITE_EFFECT_FREEZE_TIME], x, y, size, size, color);
    }

    //sprintf(buffer, "%d", cell->distance);
    //DrawText(buffer, x + 15, y + 15, 20, WHITE);
}

static inline Color LerpColor(Color a, Color b, float t) {
    Color result;
    result.r = a.r + (b.r - a.r) * t;
    result.g = a.g + (b.g - a.g) * t;
    result.b = a.b + (b.b - a.b) * t;
    result.a = a.a + (b.a - a.a) * t;
    return result;
}

static void drawLifeBar(Game* this, int x, int y, int width, int height) {
    int h = height * 0.26;
    int w = width * 0.88;
    float t = this->map->player->life / (float)(START_LIFE);
    if (t < 0.0f) t = 0.0f;
    Color color = RED;
    int lx = w * t;
    DrawRectangle(x + 50, y + h + 10, w, h, HUD_OPACITY);
    DrawRectangle(x + 50, y + h + 10, lx, h, color);
    DrawSprite(this->sprites[SPRITE_LIFE_BAR], x, y, width, height, WHITE);
}

static void drawBateryBar(Game* this, int x, int y, int width, int height) {
    if (this->map->player->biome != VIOLENCIA) return;
    int w = width * 0.95;
    int h = height * 0.35;
    int startW = width * 0.025;
    float t = this->map->player->batery;
    Color color = LerpColor(RED, GREEN, t);
    int lx = w * t;
    DrawRectangle(x + startW, y + h, w, h, HUD_OPACITY);
    DrawRectangle(x + startW, y + h, lx, h, color);
    DrawSprite(this->sprites[SPRITE_BATERY_BAR], x, y, width, height, WHITE);
    //DrawAnimation(this->animations[ANIMATION_BATERY], x, y, height, WHITE);
}

static void drawTimeHUD(Game* this, int x, int y, int width) {
    Map* map = this->map;

    int remainingTime = BIOME_DEGEN_START_TIME - map->biomeTime;
    int mm = remainingTime / 60;
    int ss = remainingTime % 60;

    sprintf(buffer, "%02d:%02d", mm, ss);

    float d = map->biomeTime / BIOME_DEGEN_START_TIME;
    int stage = min(d * 4.0f, 3);

    const static char* stageText[] = {
        "STABLE",
        "UNSTABLE",
        "CRACKING",
        "COLLAPSING"
    };

    const static Color stageColors[] = {
        { 0,   228, 48,  255 },
        { 253, 249, 0,   255 },
        { 255, 161, 0,   255 },
        { 230, 41,  55,  255 }
    };

    Color stageColor = stageColors[stage];

    int boxHeight = 80;

    DrawRectangle(x, y, width, boxHeight, HUD_OPACITY);

    drawCenteredText(buffer, x + (width >> 1), y + 10, 40, WHITE);
    drawCenteredText(stageText[stage], x + (width >> 1), y + 50, 20, stageColor);
}

static void drawInfoHud(Game* this, int x, int y, int size) {
    if (this->map->player->biome < VIOLENCIA) {
        DrawRectangle(x, y, size * 3, size * 2, HUD_OPACITY);
        DrawAnimation(this->animations[ANIMATION_COIN], x, y, size, WHITE);
        sprintf(buffer, "%d/%d", this->map->player->biomeCoins, COINS_TO_FRAGMENT);
        DrawText(buffer, x + size, y + size / 3, 30, WHITE);
        DrawAnimation(this->animations[ANIMATION_FRAGMENT], x, y + size, size, WHITE);
        sprintf(buffer, "%d/2", this->map->player->biomeFragment);
        DrawText(buffer, x + size, y + size / 3 + size, 30, WHITE);
    }
    drawTimeHUD(this, x, y + size * 2, size * 3);
}

static void drawActionHud(Game* this, Color color) {
    BeginTextureMode(this->shadowMap);
    DrawCircleGradient(this->offsetHalfX, this->offsetHalfY, this->halfDiagonal, WHITE, color);
    EndTextureMode();

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

static void drawEffects(Game* this, int x, int y, int size) {
    Sprite* sprites = this->sprites;
    //Animation* animations = this->animations;

    Effects effects = this->map->player->effects;

    Color hudColor;

    static Sprite spritesBuffer[6];
    int length = 0;

    if (effects.degeneration.duration > 0) {
        spritesBuffer[length++] = sprites[SPRITE_EFFECT_DEGENERATION];
        hudColor = RED;
    }
    if (effects.slowness.duration > 0) {
        spritesBuffer[length++] = sprites[SPRITE_EFFECT_SLOWNESS];
        hudColor = GRAY;
    }
    if (effects.regeneration.duration > 0) {
        spritesBuffer[length++] = sprites[SPRITE_EFFECT_REGENERATION];
        hudColor = (Color){ 0, 255, 0, 255 };
    }
    if (effects.invisibility.duration > 0) {
        spritesBuffer[length++] = sprites[SPRITE_EFFECT_INVISIBILITY];
        hudColor = (Color){ 0, 255, 255, 255 };
    }
    if (effects.freezeTime.duration > 0) {
        spritesBuffer[length++] = sprites[SPRITE_EFFECT_FREEZE_TIME];
        hudColor = (Color){ 0, 0, 255, 255 };
    }
    if (effects.invulnerability.duration > 0) {
        spritesBuffer[length++] = sprites[SPRITE_EFFECT_INVULNERABILITY];
        if (effects.invulnerability.duration & 1)
            hudColor = YELLOW;
        else
            hudColor = PURPLE;
    }

    int delta = size + 20;
    int ey = y;
    if (length > 0)
        drawActionHud(this, hudColor);
    for (int i = 0; i < length; i++) {
        DrawSprite(spritesBuffer[i], x, ey += delta, size, size, WHITE);
    }
}

static void drawMinimap(Game* this, int x0, int y0, int size, int zoom) {
    Map* map = this->map;
    Player* p = map->player;
    ChunkManager* cm = map->manager;
    Animation* animations = this->animations;
    Sprite* sprites = this->sprites;
    int cellSize = (size - 50) / zoom;
    DrawRectangle(x0, y0, size, size, HUD_OPACITY);
    DrawSprite(this->sprites[SPRITE_MINIMAP], x0, y0, size, size, WHITE);
    for (int y = 0; y < zoom; y++) {
        int py = map->player->y + y - (zoom >> 1);
        for (int x = 0; x < zoom; x++) {
            int px = map->player->x + x - (zoom >> 1);
            Cell* cell = cm->getLoadedCell(cm, px, py);
            drawCell(this, cell, x0 + x * cellSize + 25, y0 + y * cellSize + 25, cellSize, false);
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
            int x = (e->x - map->player->x) * cellSize + 25 + x0 + offset;
            int y = (e->y - map->player->y) * cellSize + 25 + y0 + offset;
            int size = cellSize;

            Color color = BIOME_COLOR[e->biome];
            if (p->effects.invulnerability.duration > 0 && this->updateCount & 1) color = getNegativeColor(color);

            if (e->isBoss) {
                x -= cellSize;
                y -= cellSize;
                size *= 3;
            }

            DrawAnimation(animations[ANIMATION_PACMAN_RIGHT + e->dir], x, y, size, color);

            if (p->effects.freezeTime.duration > 0)
                DrawSprite(sprites[SPRITE_ICE], x, y, size, size, (Color) { 255, 255, 255, 150 });

            cur = cur->next;
        }
    }

    // for (int i = -1; i < 2; i++) {
    //     int chunkY = map->player->chunkY + i;
    //     if (chunkY < 0 || chunkY >= map->manager->rows) continue;
    //     for (int j = -1; j < 2; j++) {
    //         int chunkX = map->player->chunkX + j;
    //         if (chunkX < 0 || chunkX >= map->manager->cols) continue;
    //         int startChunkX = x0 + offset + (chunkX * CHUNK_SIZE - map->player->x) * cellSize + 25;
    //         int startChunkY = y0 + offset + (chunkY * CHUNK_SIZE - map->player->y) * cellSize + 25;
    //         DrawRectangleLinesEx((Rectangle) { startChunkX, startChunkY, cellSize* CHUNK_SIZE + 3, cellSize* CHUNK_SIZE + 3 }, 3, GREEN);
    //     }
    // }

    DrawRectangle(x0 + offset + 25, y0 + offset + 25, cellSize, cellSize, WHITE);
}

static void drawArrowToNextBiome(Game* this, int x, int y, int width, int height) {
    DrawSprite(this->sprites[SPRITE_ARROW_NEXT_BIOME], x, y, width, height, WHITE);
    drawCenteredText("BIOMA LIBERÁDO", this->offsetHalfX, y + height * 0.41, 30, PURPLE);
}

static void drawHud(Game* this) {
    Map* map = this->map;
    Player* p = map->player;
    // Chunk* chunk = map->manager->getChunk(map->manager, p->chunkX, p->chunkY);
    // sprintf(buffer,
    //     "Life:%d\nChunk x: %d, y: %d\nCord x:%d, y:%d\ncx:%d, cy:%d\nChunkBiome: %d\nBiome:%d\nCoins:%d\nBiome Coins:%d\nFragment:%d\nBiome Fragment:%d\nInvulnerability:%d\nPlayerCell:%d\n",
    //     p->life, p->chunkX, p->chunkY, p->x, p->y, p->x & CHUNK_MASK, p->y & CHUNK_MASK, chunk->biome,
    //     p->biome, p->totalCoins, p->biomeCoins, p->totalFragment, p->biomeFragment, p->effects.invulnerability.duration, p->cellType);
    // DrawRectangle(20, this->height - 600, 300, 600, HUD_OPACITY);
    // DrawText(buffer, 30, this->height - 580, 30, GREEN);

    if (p->damaged) drawActionHud(this, RED);
    drawEffects(this, 30, 30, 80);

    static const char* BIOMES[4] = { "Luxuria", "Gula", "Heresia", "Violencia" };

    float def = map->biomeTime / BIOME_DEGEN_START_TIME;

    if (def < 0.1) {
        Vector2 wt = MeasureTextEx(InfernoFont, BIOMES[p->biome], 90, 10);
        Vector2 biomeNamePos = { this->offsetHalfX - wt.x * 0.5, 50 };
        DrawTextEx(InfernoFont, BIOMES[p->biome], biomeNamePos, 90, 10, BIOME_COLOR[p->biome]);
    }

    drawMinimap(this, this->width - 520, 40, 500, 80);

    if (map->manager->heaven) return;

    drawLifeBar(this, this->offsetHalfX - 300, this->height - 150, 600, 100);
    drawBateryBar(this, this->offsetHalfX - 200, this->height - 220, 400, 80);
    drawInfoHud(this, this->width - 280, 550, 80);

    if (p->biomeFragment >= 2 && this->updateCount & 4 && p->biome < VIOLENCIA)
        drawArrowToNextBiome(this, this->offsetHalfX - 300, 100, 600, 150);
}

static void playAudio(Game* this) {
    Player* p = this->map->player;
    Audio* audio = this->audio;

    if (p->cellType == CELL_HEAVEN) {
        audio->updateMusic(audio, MUSIC_HEAVEN);
        return;
    }

    if (p->effects.invulnerability.duration > 0)
        audio->updateMusic(audio, MUSIC_INVULNERABILITY);
    else if (p->effects.freezeTime.duration > 0)
        audio->updateMusic(audio, MUSIC_FREEZE_TIME);
    else if (p->effects.invisibility.duration > 0)
        audio->updateMusic(audio, MUSIC_INVISIBILITY);
    else {
        if (this->map->biomeTime < BIOME_DEGEN_START_TIME)
            audio->updateMusic(audio, p->biome + MUSIC_LUXURIA);
        else
            audio->updateMusic(audio, MUSIC_DEGENERATION);
    }

    if (p->effects.invulnerability.duration == FRUIT_INVULNERABILITY_DURATION - 1)
        audio->restartMusic(audio, MUSIC_INVULNERABILITY);
    if (p->effects.freezeTime.duration == FREEZE_TIME_DURATION - 1)
        audio->restartMusic(audio, MUSIC_FREEZE_TIME);
    if (p->effects.invisibility.duration == INVISIBILITY_DURATION - 1)
        audio->restartMusic(audio, MUSIC_INVISIBILITY);

    if (this->frameCount != this->lastUpdate) return;

    CellType type = p->cellType;

    if (p->damaged) {
        audio->playSound(audio, SOUND_DAMAGE);
    }
    if (p->hitEnemy) {
        audio->playSound(audio, SOUND_KILL_ENEMY);
    }

    if (p->biomeFragment >= 2) {
        audio->playSound(audio, SOUND_BIOME_FREE);
    }

    if (p->effects.regeneration.duration > 0 && p->life < START_LIFE)
        audio->playSound(audio, SOUND_REGENERATE);

    if (type == CELL_COIN) {
        audio->playSound(audio, SOUND_COIN);
    } else if (isWind(type)) {
        audio->playSound(audio, SOUND_WIND);
    } else if (type == CELL_FRAGMENT) {
        audio->playSound(audio, SOUND_FRAGMENT);
    } else if (type == CELL_FRUIT) {
        audio->playSound(audio, SOUND_FRUIT);
    } else if (type == CELL_SPIKE) {
        audio->playSound(audio, SOUND_SPIKE);
    } else if (type == CELL_FREEZE_TIME) {
        audio->playSound(audio, SOUND_FREEZE_TIME);
    } else if (type == CELL_BATERY) {
        audio->playSound(audio, SOUND_BATERY);
    } else if (type == CELL_MUD) {
        audio->playSound(audio, SOUND_MUD);
    } else if (type == CELL_INVISIBILITY || type == CELL_REGENERATION) {
        audio->playSound(audio, SOUND_POTION);
    } else if (type == CELL_TENTACLE) {
        audio->playSound(audio, SOUND_TENTACLE);
    } else if (type == CELL_FIRE_ON) {
        audio->playSound(audio, SOUND_FIRE);
    }

    if (IsKeyDown(KEY_EQUAL)) {
        audio->setSoundVolume(audio, audio->soundVolume + DELTA_VOLUME);
    }
    if (IsKeyDown(KEY_MINUS)) {
        audio->setSoundVolume(audio, audio->soundVolume - DELTA_VOLUME);
    }
    if (IsKeyDown(KEY_N)) {
        audio->setMusicVolume(audio, audio->musicVolume + DELTA_VOLUME);
    }
    if (IsKeyDown(KEY_B)) {
        audio->setMusicVolume(audio, audio->musicVolume - DELTA_VOLUME);
    }
}

static inline void drawPlayer(Game* this) {
    Player* p = this->map->player;
    Color color = WHITE;
    if (p->damaged) color = RED;
    if (p->effects.invisibility.duration > 0) color.a = 100;
    if (p->effects.invulnerability.duration > 0 && this->frameCount - this->lastUpdate < HALF_FRAMES_ANIMATION) color = PURPLE;
    DrawAnimation(this->animations[ANIMATION_GHOST_RIGHT + p->dir], this->offsetHalfX, this->offsetHalfY, this->cellSize, color);
}

static void inline drawChunksMap(Game* this, int x, int y) {
    Map* map = this->map;
    for (int i = -1; i < 2; i++) {
        int chunkY = map->player->chunkY + i;
        if (chunkY < 0 || chunkY >= map->manager->rows) continue;
        for (int j = -1; j < 2; j++) {
            int chunkX = map->player->chunkX + j;
            if (chunkX < 0 || chunkX >= map->manager->cols) continue;

            int startChunkX = x + (chunkX * CHUNK_SIZE - map->player->lastX) * this->cellSize;
            int startChunkY = y + (chunkY * CHUNK_SIZE - map->player->lastY) * this->cellSize;
            DrawRectangleLinesEx((Rectangle) { startChunkX, startChunkY, this->cellSize* CHUNK_SIZE + 5, this->cellSize* CHUNK_SIZE + 5 }, 5, GREEN);
        }
    }
}

static void drawMap(Game* this) {
    Sprite* sprites = this->sprites;
    Animation* animations = this->animations;
    Map* map = this->map;
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
        const int halfCell = this->cellSize >> 1;
        DrawCircleGradient(this->offsetHalfX + halfCell, this->offsetHalfY + halfCell, this->offsetHalfX * p->batery + 200, WHITE, BLANK);
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
                DrawCircleGradient(x, y, 200, WHITE, BLANK);
                EndTextureMode();
            }

            Color color = BIOME_COLOR[e->biome];
            if (p->effects.invulnerability.duration > 0 && this->updateCount & 1) color = getNegativeColor(color);

            int size = this->cellSize;

            if (e->isBoss) {
                x -= size;
                y -= size;
                size *= 3;
            }

            DrawAnimation(animations[ANIMATION_PACMAN_RIGHT + e->dir], x, y, size, color);

            if (p->effects.freezeTime.duration > 0)
                DrawSprite(sprites[SPRITE_ICE], x, y, size, size, (Color) { 255, 255, 255, 150 });

            cur = cur->next;
        }
    }

    if (p->biome == VIOLENCIA && !map->manager->heaven) {
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

    drawPlayer(this);

    //drawChunksMap(this, offsetHalfXAnimated, offsetHalfYAnimated);

    drawHud(this);
    playAudio(this);

    this->frameCount++;
}

static bool deathSoundPlayed = false;
static void updateDeathScreen(Game* this) {
    Vector2 mouse = GetMousePosition();
    Audio* audio = this->audio;

    if (!deathSoundPlayed) {
        audio->playSound(audio, SOUND_DEATH);
        deathSoundPlayed = true;
    }

    audio->updateMusic(audio, MUSIC_GAME_OVER);

    Button* buttons[2] = { this->backMenu, this->restartGame };

    for (int i = 0; i < 2; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            deathSoundPlayed = false;
            if (b->action) b->action();
        }
    }
}

static void update(Game* this) {
    if (state == GAME_DEATH) updateDeathScreen(this);
}

static void drawDeathScreen(Game* this) {
    DrawSprite(this->sprites[SPRITE_GAME_OVER], 0, 0, this->width, this->height, WHITE);
    this->backMenu->draw(this->backMenu);
    this->restartGame->draw(this->restartGame);
}

static void draw(Game* this) {
    if (state == GAME_MAIN_CONTENT) drawMap(this);
    else if (state == GAME_DEATH) drawDeathScreen(this);
}

static void loadSprites(Game* this) {
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

    const char* coin[] = { "assets/sprites/itens/coin1.png", "assets/sprites/itens/coin2.png", "assets/sprites/itens/coin3.png", "assets/sprites/itens/coin4.png",
    "assets/sprites/itens/coin5.png", "assets/sprites/itens/coin6.png" };
    const char* fragment[] = { "assets/sprites/itens/newKey.png" , "assets/sprites/itens/newKey2.png", "assets/sprites/itens/newKey3.png", "assets/sprites/itens/newKey4.png",
         "assets/sprites/itens/newKey5.png", "assets/sprites/itens/newKey4.png", "assets/sprites/itens/newKey3.png", "assets/sprites/itens/newKey2.png" };

    animations[ANIMATION_COIN] = LoadAnimation(6, coin);
    animations[ANIMATION_FRAGMENT] = LoadAnimation(8, fragment);

    const char* verticalWind[] = { "assets/sprites/luxuria/ventania1.png", "assets/sprites/luxuria/ventania2.png" };
    const char* horizontalWind[] = { "assets/sprites/luxuria/ventania3.png", "assets/sprites/luxuria/ventania4.png" };
    const char* mud[] = { "assets/sprites/gula/lama1.png", "assets/sprites/gula/lama2.png", "assets/sprites/gula/lama3.png" };
    const char* fire[] = { "assets/sprites/heresia/fogo.png", "assets/sprites/heresia/fogo2.png", "assets/sprites/heresia/fogo3.png",
                             "assets/sprites/heresia/fogo4.png" };

    const char* tentacle[] = { "assets/sprites/common_cells/tentacle1.png", "assets/sprites/common_cells/tentacle2.png", "assets/sprites/common_cells/tentacle3.png",
    "assets/sprites/common_cells/tentacle4.png" };

    const char* font[] = { "assets/sprites/common_cells/fonte.png", "assets/sprites/common_cells/fonte1.png", "assets/sprites/common_cells/fonte2.png", "assets/sprites/common_cells/fonte3.png",
                            "assets/sprites/common_cells/fonte3.png", "assets/sprites/common_cells/fonte2.png", "assets/sprites/common_cells/fonte1.png" };

    const char* batery[] = { "assets/sprites/itens/batery1.png", "assets/sprites/itens/batery2.png", "assets/sprites/itens/batery3.png", "assets/sprites/itens/batery4.png" };

    const char* spike[] = { "assets/sprites/violencia/spike1.png", "assets/sprites/violencia/spike2.png", "assets/sprites/violencia/spike3.png" };

    const char* portal[] = { "assets/sprites/common_cells/portal1.png", "assets/sprites/common_cells/portal2.png", "assets/sprites/common_cells/portal3.png" };

    animations[ANIMATION_HORIZONTAL_WIND] = LoadAnimation(2, horizontalWind);
    animations[ANIMATION_VERTICAL_WIND] = LoadAnimation(2, verticalWind);
    animations[ANIMATION_MUD] = LoadAnimation(3, mud);
    animations[ANIMATION_FIRE] = LoadAnimation(4, fire);
    animations[ANIMATION_TENTACLE] = LoadAnimation(4, tentacle);
    animations[ANIMATION_BATERY] = LoadAnimation(4, batery);

    animations[ANIMATION_FONT] = LoadAnimation(7, font);
    animations[ANIMATION_SPIKE] = LoadAnimation(3, spike);

    animations[ANIMATION_PORTAL] = LoadAnimation(3, portal);

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

    sprites[SPRITE_HEAVEN] = LoadSprite("assets/sprites/common_cells/heaven.png");

    sprites[SPRITE_GRAVE] = LoadSprite("assets/sprites/heresia/cova.png");

    sprites[SPRITE_ICE] = LoadSprite("assets/sprites/common_cells/ice.png");

    sprites[SPRITE_EFFECT_REGENERATION] = LoadSprite("assets/sprites/effects/regeneration.png");
    sprites[SPRITE_EFFECT_DEGENERATION] = LoadSprite("assets/sprites/effects/degeneration.png");
    sprites[SPRITE_EFFECT_SLOWNESS] = LoadSprite("assets/sprites/effects/slowness.png");
    sprites[SPRITE_EFFECT_INVISIBILITY] = LoadSprite("assets/sprites/effects/invisibility.png");
    sprites[SPRITE_EFFECT_FREEZE_TIME] = LoadSprite("assets/sprites/effects/time.png");
    sprites[SPRITE_EFFECT_INVULNERABILITY] = LoadSprite("assets/sprites/effects/star.png");

    sprites[SPRITE_MINIMAP] = LoadSprite("assets/sprites/hud/minimap.png");
    sprites[SPRITE_LIFE_BAR] = LoadSprite("assets/sprites/hud/lifebar.png");
    sprites[SPRITE_BATERY_BAR] = LoadSprite("assets/sprites/hud/batery_hud.png");
    sprites[SPRITE_ARROW_NEXT_BIOME] = LoadSprite("assets/sprites/hud/biomeNextArrow.png");

    sprites[SPRITE_GAME_OVER] = LoadSprite("assets/sprites/hud/game_over.jpg");
}

static void loadSounds(Game* this) {
    Audio* audio = new_Audio(MUSIC_COUNT, SOUND_COUNT);
    this->audio = audio;
    audio->setSoundVolume(audio, 0.3);
    audio->setMusicVolume(audio, 0.5);

    audio->loadMusic(audio, "assets/music/luxuria_trilha.mp3", MUSIC_LUXURIA);
    audio->loadMusic(audio, "assets/music/gula_trilha.mp3", MUSIC_GULA);
    audio->loadMusic(audio, "assets/music/heresia_trilha.mp3", MUSIC_HERESIA);
    audio->loadMusic(audio, "assets/music/violencia_trilha.mp3", MUSIC_VIOLENCIA);

    audio->loadMusic(audio, "assets/music/Invincibility_mario.mp3", MUSIC_INVULNERABILITY);
    audio->loadMusic(audio, "assets/music/invisibility.mp3", MUSIC_INVISIBILITY);
    audio->loadMusic(audio, "assets/music/freeze.mp3", MUSIC_FREEZE_TIME);

    audio->loadMusic(audio, "assets/music/earthquake.mp3", MUSIC_DEGENERATION);
    audio->loadMusic(audio, "assets/music/heaven.mp3", MUSIC_HEAVEN);

    audio->loadMusic(audio, "assets/music/game_over.mp3", MUSIC_GAME_OVER);

    audio->loadSound(audio, "assets/sounds/regenerate.wav", SOUND_REGENERATE);
    audio->loadSound(audio, "assets/sounds/moedinha.wav", SOUND_COIN);
    audio->loadSound(audio, "assets/sounds/ventania2.wav", SOUND_WIND);
    audio->loadSound(audio, "assets/sounds/dano.wav", SOUND_DAMAGE);
    audio->loadSound(audio, "assets/sounds/killEnemy.wav", SOUND_KILL_ENEMY);
    audio->loadSound(audio, "assets/sounds/fragmento.wav", SOUND_FRAGMENT);
    audio->loadSound(audio, "assets/sounds/fruit.wav", SOUND_FRUIT);
    audio->loadSound(audio, "assets/sounds/spike.wav", SOUND_SPIKE);
    audio->loadSound(audio, "assets/sounds/time.wav", SOUND_FREEZE_TIME);
    audio->loadSound(audio, "assets/sounds/potion.wav", SOUND_POTION);
    audio->loadSound(audio, "assets/sounds/batery.wav", SOUND_BATERY);
    audio->loadSound(audio, "assets/sounds/mud.wav", SOUND_MUD);
    audio->loadSound(audio, "assets/sounds/biomeFree.wav", SOUND_BIOME_FREE);
    audio->loadSound(audio, "assets/sounds/tentacle.wav", SOUND_TENTACLE);
    audio->loadSound(audio, "assets/sounds/fire.wav", SOUND_FIRE);

    audio->loadSound(audio, "assets/sounds/click.wav", SOUND_CLICK_BUTTON);
    audio->loadSound(audio, "assets/sounds/death.wav", SOUND_DEATH);
}

static Map* thisMap;
static void backToMenu() {
    thisMap->restart(thisMap);
    state = MENU_MAIN_CONTENT;
}

static void restartGame() {
    thisMap->restart(thisMap);
    state = GAME_MAIN_CONTENT;
}

static void loadButtons(Game* this) {
    int btnW = 160;
    int btnH = 50;
    int margin = 40;
    int y = this->height - btnH - margin;
    this->backMenu = new_Button(
        margin,
        y,
        btnW,
        btnH,
        (Color) {
        0, 0, 0, 180
    },
        (Color) {
        255, 255, 255, 200
    },
        "MENU",
        30,
        backToMenu
    );

    this->restartGame = new_Button(
        this->width - btnW - margin,
        y,
        btnW,
        btnH,
        (Color) {
        0, 0, 0, 180
    },
        (Color) {
        255, 255, 255, 200
    },
        "RESTART",
        30,
        restartGame
    );
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

    free(this->animations);

    for (int i = 0; i < SPRITE_COUNT; i++) {
        UnloadSprite(this->sprites[i]);
    }

    free(this->sprites);

    this->audio->free(this->audio);
    UnloadRenderTexture(this->shadowMap);
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

    this->halfDiagonal = hypotf(width >> 1, height >> 1);

    this->offsetHalfX = width >> 1;
    this->offsetHalfY = height >> 1;

    this->map = map;
    thisMap = map;

    this->animations = malloc(sizeof(Animation) * ANIMATION_COUNT);
    this->sprites = malloc(sizeof(Sprite) * SPRITE_COUNT);

    loadSounds(this);
    loadSprites(this);
    loadButtons(this);

    this->shadowMap = LoadRenderTexture(this->width, this->height);

    this->draw = draw;
    this->update = update;
    this->saveUpdate = saveUpdate;
    this->free = _free;
    return this;
}
