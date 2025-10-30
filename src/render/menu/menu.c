#include "menu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

void _free(Menu* this){
    Button* play = this->play;
    play->free(play);
    free(this);
}

void play(){
    state = GAME;
}

void draw(Menu* this){
    Button* play = this->play;
    play->draw(play);
}

void update(Menu* this){
    Button* play = this->play;
    Vector2 mouse = GetMousePosition();
    play->hovered = play->isInside(play, mouse);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && play->hovered)
        play->action(play);
}

Menu* new_Menu(){
    Menu* this = malloc(sizeof(Menu));
    this->play = new_Button(100, 100, 200, 100, RED, GREEN, "PLAY", 20, play);

    this->draw = draw;
    this->update = update;
    this->free = _free;
    return this;
}