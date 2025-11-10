#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include "sounds.h"
#include "map.h"

#define FRAMES_ANIMATION 8.0f

typedef enum {
    BIOME_FLOOR_SPRITE,
    BIOME_WALL_SPRITE,
    BIOME_ITEN_1_SPRITE,
    BIOME_ITEN_2_SPRITE
} BiomeSprite;

typedef struct Sprites {
    Texture2D ghost[8];
    Texture2D pacman[8];

    Texture2D itens[3];

    Texture2D gula[3];
    Texture2D heresia[4];
    Texture2D flegetonte[3];
    Texture2D luxuria[3];
} Sprites;

typedef struct Game {
    int width;
    int height;

    int cellSize;

    int renderDistX;
    int renderDistY;

    int offsetHalfX;
    int offsetHalfY;

    unsigned int lastUpdate;
    unsigned int frameCount;
    unsigned int updateCount;

    Map* map;
    Chunk* visibleChunks[49];

    Sounds* sounds;

    RenderTexture2D shadowMap;

    Sprites sprites;

    void (*drawMapDebug)(struct Game*);
    void (*saveUpdate)(struct Game*);
    void (*free)(struct Game*);
} Game;

Game* new_Game(int width, int height, int cellSize, Map* map);

#endif