#include "sprites.h"
#include <stdlib.h>

void UpdateAnimation(Animation* animation) {
    animation->actualFrame++;
    if (animation->actualFrame == animation->lenght) animation->actualFrame = 0;
}

void DrawAnimation(Animation animation, int x, int y, int size) {
    Texture2D frame = animation.frames[animation.actualFrame];
    Rectangle source = { 0, 0, frame.width, frame.height };
    Rectangle dest = { x, y, size, size };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(frame, source, dest, origin, 0.0f, WHITE);
}

Texture2D GetAnimationFrame(Animation animation) {
    return animation.frames[animation.actualFrame];
}

void UnloadAnimation(Animation animation) {
    for (int i = 0; i < animation.lenght; i++) {
        UnloadTexture(animation.frames[i]);
    }

    free(animation.frames);
}

Animation LoadAnimation(int lenght, const char** path) {
    Animation this = { 0 };

    this.lenght = lenght;
    this.actualFrame = 0;
    this.frames = malloc(sizeof(Texture2D) * lenght);

    for (int i = 0; i < lenght; i++) {
        this.frames[i] = LoadTexture(path[i]);
    }

    return this;
}

void DrawSprite(Sprite sprite, int x, int y, int size) {
    Texture2D texture = sprite.texture;
    Rectangle source = { 0, 0, texture.width, texture.height };
    Rectangle dest = { x, y, size, size };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
}

void UnloadSprite(Sprite sprite) {
    UnloadTexture(sprite.texture);
}

Sprite LoadSprite(const char* path) {
    Sprite this = { 0 };
    this.texture = LoadTexture(path);

    return this;
}