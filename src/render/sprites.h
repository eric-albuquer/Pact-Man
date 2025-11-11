#ifndef SPRITES_H
#define SPRITES_H

#include <raylib.h>

typedef struct Sprite {
    Texture2D texture;
} Sprite;

void DrawSprite(Sprite sprite, int x, int y, int size);
void UnloadSprite(Sprite sprite);
Sprite LoadSprite(const char* path);

typedef struct Animation {
    Texture2D* frames;
    int lenght;
    int actualFrame;
} Animation;

Animation LoadAnimation(int lenght, const char** path);
void UpdateAnimation(Animation* animation);
Texture2D GetAnimationFrame(Animation animation);
void DrawAnimation(Animation animation, int x, int y, int size);
void UnloadAnimation(Animation animation);

#endif