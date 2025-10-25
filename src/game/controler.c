#include "controler.h"
#include <stdlib.h>

static void reset(Controler* this){
    this->dx = this->dy = 0;
}

static void getInputs(Controler* this) {
    if (IsKeyDown(KEY_W))
        this->dy = -1;
    else if (IsKeyDown(KEY_S))
        this->dy = 1;
    else if (IsKeyDown(KEY_A))
        this->dx = -1;
    else if (IsKeyDown(KEY_D))
        this->dx = 1;
}

static void _free(Controler* this) { free(this); }

Controler* new_Controler() {
    Controler* this = malloc(sizeof(Controler));
    reset(this);
    this->getInputs = getInputs;
    this->reset = reset;
    this->free = _free;
    return this;
}