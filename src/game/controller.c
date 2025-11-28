#include "controller.h"
#include <stdlib.h>

//===============================================================
//  REINICIAR CONTROLE
//===============================================================

static void reset(Controller* this) {
    int lastCode = this->input.code;
    this->input = (Input){ 0 };
    this->input.lastCode = lastCode;
}

//===============================================================
//  RECEBER INPUTS DO TECLADO
//===============================================================

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

    if (IsKeyDown(KEY_SPACE)) {
        this->input.speed = 1;
    }

    if (IsKeyDown(KEY_ENTER)) {
        this->input.pause = 1;
    }

    if (this->input.code == 0)
        this->input.code = GetKeyPressed();
}

//===============================================================
//  RECEBER INPUTS DO CONTROLE
//===============================================================

static void getControllerInputs(Controller* this) {
    if (!IsGamepadAvailable(0)) return;

    if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) > MIN_SENSITIVITY) {
        this->input.speed = 1;
        this->input.code = GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
    }

    if (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) > MIN_SENSITIVITY){
        this->input.code = GAMEPAD_BUTTON_LEFT_TRIGGER_2;
    }

    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
        this->input.pause = 1;
    }

    float horizontal = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    float vertical = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

    if (horizontal > MIN_SENSITIVITY || GetGamepadButtonPressed() == GAMEPAD_BUTTON_LEFT_FACE_RIGHT) {
        this->input.right = 1;
    } else if (horizontal < -MIN_SENSITIVITY || GetGamepadButtonPressed() == GAMEPAD_BUTTON_LEFT_FACE_LEFT) {
        this->input.left = 1;
    }

    if (vertical > MIN_SENSITIVITY || GetGamepadButtonPressed() == GAMEPAD_BUTTON_LEFT_FACE_DOWN) {
        this->input.down = 1;
    } else if (vertical < -MIN_SENSITIVITY || GetGamepadButtonPressed() == GAMEPAD_BUTTON_LEFT_FACE_UP) {
        this->input.up = 1;
    }

    if (this->input.code == 0)
        this->input.code = GetGamepadButtonPressed();
}

//===============================================================
//  RECEBER QUALQUER INPUT
//===============================================================

static void getInputs(Controller* this) {
    getKeyboardInputs(this);
    getControllerInputs(this);
}

//===============================================================
//  LIBERAR MEMÓRIA
//===============================================================

static void _free(Controller* this) {
    free(this);
}

//===============================================================
//  CONSTRUTOR
//===============================================================

Controller* new_Controller() {
    Controller* this = malloc(sizeof(Controller));
    reset(this);
    this->input.lastCode = 0;

    this->getInputs = getInputs;
    this->reset = reset;
    this->free = _free;

    return this;
}