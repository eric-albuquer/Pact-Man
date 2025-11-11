#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include "sounds.h"
#include "sprites.h"
#include "map.h"

#define FRAMES_ANIMATION 8.0f

typedef enum {
    SPRITE_COIN,
    SPRITE_FRAGMENT,
    SPRITE_FRUIT
} ItenSprites;

typedef struct Sprites {
    Animation ghost[4];
    Animation pacman[4];

    Animation itens[3];

    Sprite floor[4];
    Sprite wall[4];
    
    Animation wind;
    Sprite mud;
    Sprite grave;
    Sprite fire;
    Sprite spike;
    
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

    Sounds* sounds;

    RenderTexture2D shadowMap;

    Sprites sprites;

    void (*drawMapDebug)(struct Game*);
    void (*saveUpdate)(struct Game*);
    void (*free)(struct Game*);
} Game;

Game* new_Game(int width, int height, int cellSize, Map* map);

#endif