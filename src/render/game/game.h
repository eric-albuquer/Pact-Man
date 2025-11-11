#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include "sounds.h"
#include "sprites.h"
#include "map.h"

#define FRAMES_ANIMATION 8.0f

typedef enum {
    SPRITE_WALL_LUXURIA,
    SPRITE_WALL_GULA,
    SPRITE_WALL_HERESIA,
    SPRITE_WALL_VIOLENCIA,

    SPRITE_FLOOR_LUXURIA,
    SPRITE_FLOOR_GULA,
    SPRITE_FLOOR_HERESIA,
    SPRITE_FLOOR_VIOLENCIA,

    SPRITE_MUD,
    SPRITE_GRAVE,
    SPRITE_SPIKE,

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
    ANIMATION_FRUIT,

    ANIMATION_WIND,
    ANIMATION_FIRE,

    ANIMATION_COUNT
} AnimationsEnum;

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

    Animation animations[ANIMATION_COUNT];
    Sprite sprites[SPRITE_COUNT];

    void (*drawMapDebug)(struct Game*);
    void (*saveUpdate)(struct Game*);
    void (*free)(struct Game*);
} Game;

Game* new_Game(int width, int height, int cellSize, Map* map);

#endif