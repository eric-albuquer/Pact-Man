#ifndef GAME_H
#define GAME_H

#include <raylib.h>

#include "map.h"

#define FRAMES_ANIMATION 12.0f

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

    Texture2D* sprites;
    unsigned int totalSprites;

    void (*drawMapDebug)(struct Game*);
    void (*saveUpdate)(struct Game*);
    void (*free)(struct Game*);
} Game;

Game* new_Game(int width, int height, int cellSize, Map* map);

#endif