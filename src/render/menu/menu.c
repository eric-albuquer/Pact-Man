#include "menu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void _free(Menu* this){
    Button* play = this->play;
    play->free(play);
    free(this);
}

void play(){
    printf("Jogando!\n");
}

void draw(Menu* this){
    Button* play = this->play;
    play->draw(play);
}

void update(Menu* this){
    return;
}

Menu* new_Menu(){
    Menu* this = malloc(sizeof(Menu));
    this->play = new_Button(100, 100, 200, 100, RED, GREEN, "PLAY", 20, play);

    this->draw = draw;
    this->free = _free;
    return this;
}