#include "sprites.h"
#include <stdlib.h>

static void _free(Animation* this){
    for (int i = 0; i < this->lenght; i++){
        UnloadTexture(this->frames[i]);
    }

    free(this->frames);
    free(this);
}

static void update(Animation* this){
    this->actualFrame++;
    if (this->actualFrame == this->lenght) this->actualFrame = 0;
}

static void draw(Animation* this, int x, int y, int size){
    Texture2D frame = this->frames[this->actualFrame];
    Rectangle source = {0, 0, frame.width, frame.height};
    Rectangle dest = {x, y, size, size};
    Vector2 origin = {0, 0};

    DrawTexturePro(frame, source, dest, origin, 0.0f, WHITE);
}

Animation* new_Animation(int lenght, const char** path){
    Animation* this = malloc(sizeof(Animation));

    this->lenght = lenght;
    this->actualFrame = 0;
    this->frames = malloc(sizeof(Texture2D) * lenght);

    for (int i = 0; i < lenght; i++){
        this->frames[i] = LoadTexture(path[i]);
    }

    return this;
}

