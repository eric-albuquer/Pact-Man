#ifndef BUTTON_H
#define BUTTON_H

#include <raylib.h>

typedef struct Button {
    int x;
    int y;

    int width;
    int height;

    Color color;
    Color fontColor;
    int fontSize;
    char text[100];

    bool selected;
    bool hovered;

    bool (*isInside)(struct Button*, Vector2);
    void (*action)();

    void (*draw)(struct Button*);
    void (*free)(struct Button*);
} Button;

Button* new_Button(int x, int y, int w, int h, Color color, Color fontColor, char* text, int fontSize, void (*action)());

#endif