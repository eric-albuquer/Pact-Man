#ifndef MENU_H
#define MENU_H

#include <raylib.h>
#include "button.h"

typedef struct Menu {
    Button* play;

    void (*draw)(struct Menu*);
    void (*update)(struct Menu*);
    void (*free)(struct Menu*);
} Menu;

Menu* new_Menu();

#endif