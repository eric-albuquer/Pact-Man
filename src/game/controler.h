#ifndef CONTROLER_H
#define CONTROLER_H

#include "raylib.h"

typedef struct Controler {
    int dx;
    int dy;

    void (*getInputs)(struct Controler*);
    void (*reset)(struct Controler*);
    void (*free)(struct Controler*);
} Controler;

Controler* new_Controler();

#endif