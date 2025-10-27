#ifndef CONTROLER_H
#define CONTROLER_H

#include "raylib.h"
#include "linkedlist.h"

typedef struct Input {
    int dx;
    int dy;
} Input;

typedef struct Controler {
    LinkedList* inputBuffer;

    void (*getInputs)(struct Controler*);
    void (*reset)(struct Controler*);
    void (*free)(struct Controler*);
} Controler;

Controler* new_Controler();

#endif