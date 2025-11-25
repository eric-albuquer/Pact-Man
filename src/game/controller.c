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
    if (!IsGamepadAvailable(0)) return;

    float horizontal = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float vertical = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

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
    free(this);
}

Controller* new_Controller() {
    Controller* this = malloc(sizeof(Controller));
    reset(this);

    this->getInputs = getInputs;
    this->reset = reset;
    this->free = _free;

    return this;
}