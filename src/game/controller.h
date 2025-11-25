#ifndef CONTROLLER_H
#define CONTROLLER_H

#define SDL_MAIN_HANDLED

#include "SDL2/SDL.h"

#include "raylib.h"

#define MIN_SENSITIVITY 8000

typedef struct Input {
    unsigned right: 1;
    unsigned left: 1;
    unsigned up: 1;
    unsigned down: 1;
} Input;

typedef struct Controller {
    Input input;
    SDL_Joystick* joystick;

    void (*getInputs)(struct Controller*);
    void (*reset)(struct Controller*);
    void (*free)(struct Controller*);
} Controller;

Controller* new_Controller();

#endif