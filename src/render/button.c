#include "button.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

void freeButton(Button* this){
    free(this);
}

bool isInside(Button* this, Vector2 pos){
    return pos.x > this->x && pos.x < this->x + this->width && pos.y > this->y && pos.y < this->y + this->height;
}

void drawButton(Button* this) {
    Color color = this->hovered ? this->fontColor : this->color;
    //DrawRectangle(this->x, this->y, this->width, this->height, color);
    DrawRectangleRounded((Rectangle){this->x, this->y, this->width, this->height}, 0.7f, 16, color);

    int textWidth = MeasureText(this->text, this->fontSize);
    int textX = this->x + (this->width - textWidth) / 2;
    int textY = this->y + (this->height - this->fontSize) / 2;

    Color textColor = this->hovered ? WHITE : this->fontColor;

    //DrawTextEx(InfernoFont, this->text, (Vector2){textX, textY}, this->fontSize, 5, textColor);
    DrawText(this->text, textX, textY, this->fontSize, textColor);
}


Button* new_Button(int x, int y, int w, int h, Color color, Color fontColor, char* text, int fontSize, void (*action)()){
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
