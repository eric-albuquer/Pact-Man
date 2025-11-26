#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "raylib.h"

#define MIN_SENSITIVITY 0.5f

typedef struct Input {
    unsigned right: 1;
    unsigned left: 1;
    unsigned up: 1;
    unsigned down: 1;

    unsigned speed: 1;
    unsigned pause: 1;
} Input;

typedef struct Controller {
    Input input;

    void (*getInputs)(struct Controller*);
    void (*reset)(struct Controller*);
    void (*free)(struct Controller*);
} Controller;

Controller* new_Controller();

#endif