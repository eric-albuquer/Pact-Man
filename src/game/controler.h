#ifndef CONTROLER_H
#define CONTROLER_H

#include "raylib.h"

typedef struct Input {
    unsigned right: 1;
    unsigned left: 1;
    unsigned up: 1;
    unsigned down: 1;
} Input;

typedef struct Controler {
    Input input;

    void (*getInputs)(struct Controler*);
    void (*reset)(struct Controler*);
    void (*free)(struct Controler*);
} Controler;

Controler* new_Controler();

#endif