#include "controller.h"
#include <stdio.h>
#include <stdlib.h>

static void reset(Controller* this) { this->input = (Input){ 0 }; }

static void getKeyboardInputs(Controller* this) {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        this->input.up = 1;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        this->input.down = 1;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        this->input.left = 1;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        this->input.right = 1;
    }
}

static void getControllerInputs(Controller* this) {
    if (!this->joystick) return;

    SDL_JoystickUpdate();

    // int numButtons = SDL_JoystickNumButtons(this->joystick);
    // int numAxes = SDL_JoystickNumAxes(this->joystick);

    // Debug de botões
    // for (int i = 0; i < numButtons; i++) {
    //     if (SDL_JoystickGetButton(this->joystick, i)) {
    //         printf("BOTAO PRESSIONADO: ID = %d\n", i);
    //     }
    // }

    // Debug de eixos (analógicos)
    // for (int i = 0; i < numAxes; i++) {
    //     int value = SDL_JoystickGetAxis(this->joystick, i);

    //     if (value > 8000 || value < -8000) {
    //         printf("EIXO %d = %d\n", i, value);
    //     }
    // }

    int horizontal = SDL_JoystickGetAxis(this->joystick, 0);
    int vertical = SDL_JoystickGetAxis(this->joystick, 1);

    if (horizontal > MIN_SENSITIVITY)
        this->input.right = 1;
    else if (horizontal < -MIN_SENSITIVITY)
        this->input.left = 1;

    if (vertical > MIN_SENSITIVITY)
        this->input.down = 1;
    else if (vertical < -MIN_SENSITIVITY)
        this->input.up = 1;
}


static void getInputs(Controller* this) {
    getKeyboardInputs(this);
    getControllerInputs(this);
}

static void _free(Controller* this) {
    if (this->joystick) {
        SDL_JoystickClose(this->joystick);
        this->joystick = NULL;
    }

    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    
    free(this);
}

Controller* new_Controller() {
    Controller* this = malloc(sizeof(Controller));
    reset(this);

    SDL_Init(SDL_INIT_JOYSTICK);
    this->joystick = SDL_JoystickOpen(0);

    this->getInputs = getInputs;
    this->reset = reset;
    this->free = _free;

    return this;
}