#include "exitpoput.h"
#include "button.h"
#include "common.h"

static Button* btnExit;
static Button* btnCancel;

static void cancelFunc(){}

static int margin = 70;
static int btnWidth = 300;
static int btnHeight = 100;
static int btnFontSize = 40;

void loadPopup(int width, int height, void (*exit)()) {
    const int hw = width >> 1;
    const int hh = height >> 1;
    const int hm = margin >> 1;
    btnExit = new_Button(hw + hm, hh, btnWidth, btnHeight, RED, BLACK, "SAIR", btnFontSize, exit);
    btnCancel = new_Button(hw - (btnWidth + hm), hh, btnWidth, btnHeight, WHITE, BLACK, "CANCELAR", btnFontSize, cancelFunc);
}

void unloadPopup() {
    btnExit->free(btnExit);
    btnCancel->free(btnCancel);
}

void drawPopup() {
    const int w = (btnExit->x + margin + btnExit->width) - (btnCancel->x - margin);
    const int h = btnCancel->height + (margin << 1);
    Color backgroundColor = {127, 127, 127, 200};
    DrawRectangleRounded((Rectangle){btnCancel->x - margin, btnCancel->y - margin, w, h}, 0.5f, 16, backgroundColor);
    btnExit->draw(btnExit);
    btnCancel->draw(btnCancel);
}

bool updatePopup() {
    Button* buttons[2] = {btnExit, btnCancel};
    return updateButtons(buttons, 2);
}

void setPopupY(int y){
    btnExit->y = y;
    btnCancel->y = y;
}