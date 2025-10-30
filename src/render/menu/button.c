#include "button.h"
#include <stdlib.h>
#include <string.h>

void freeButton(Button* this){
    free(this);
}

bool isInside(Button* this, int x, int y){
    return x > this->x && x < this->x + this->width && y > this->y && y < this->y + this->height;
}

void drawButton(Button* this){
    DrawRectangle(this->x, this->y, this->width, this->height, this->color);
    int halfX = this->x + (this->width >> 1);
    int halfY = this->y + (this->height >> 1);
    DrawText(this->text, halfX, halfY, this->fontSize, this->fontColor);
}

Button* new_Button(int x, int y, int w, int h, Color color, Color fontColor, char* text, int fontSize, void (*action)(Button*)){
    Button* this = malloc(sizeof(Button));
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
    this->color = color;
    this->fontColor = fontColor;
    this->fontSize = fontSize;
    strcpy(this->text, text);
    this->selected = false;
    this->hovered = false;

    this->action = action;
    this->isInside = isInside;
    this->draw = drawButton;
    this->free = freeButton;
    return this;
}