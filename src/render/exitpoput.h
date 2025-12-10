#ifndef EXIT_POPUT_H
#define EXIT_POPUT_H

#include <raylib.h>

void loadPopup(int width, int height, void (*exit)());

void unloadPopup();

void drawPopup();

bool updatePopup();

void setPopupY(int y);

void resetSelected();

#endif