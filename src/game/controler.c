#include "controler.h"

#include <stdlib.h>

static void reset(Controler* this) {
    this->input.dx = 0;
    this->input.dy = 0;
}

static void getInputs(Controler* this) {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)){
        this->input.dy = -1;
    } 
    else if (IsKeyDown(KEY_S)|| IsKeyDown(KEY_DOWN)){
        this->input.dy = 1;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
        this->input.dx = -1;
    }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
        this->input.dx = 1;
    }
}

static void _free(Controler* this) {
    free(this);
}

Controler* new_Controler() {
    Controler* this = malloc(sizeof(Controler));
    reset(this);
    this->getInputs = getInputs;
    this->reset = reset;
    this->free = _free;
    return this;
}