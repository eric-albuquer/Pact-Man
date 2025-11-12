#include "menu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h" 
#include <raylib.h> 
#include <math.h>

static FlameParticle flames[NUM_FLAMES];
static bool flamesInit = false;

// ---- Aqui sao os botoes ---- 
void onPlay() {
    state = GAME;
}

void onVolume() {
    printf("[MENU] Rapaz, clicaram no botão de volume\n");
}

void onDifficulty() {
    printf("[MENU] Rapaz, clicaram no botão de dificuldade\n");
}

// ---- Metodos do menu viu galera ---- :D
void _free(Menu* this){
    if (!this) return;

    if (this->play) {
        Button* b = this->play;
        b->free(b);
    }
    if (this->volume) {
        Button* b = this->volume;
        b->free(b);
    }
    if (this->difficulty) {
        Button* b = this->difficulty;
        b->free(b);
    }

    free(this);
}

static void ResetFlame(FlameParticle* f, int screenWidth, int screenHeight) {
    f->x = (float)(rand() % screenWidth);
    f->y = (float)(screenHeight + rand() % 100);   
    f->speed = 40.0f + (float)(rand() % 80);       
    f->radius = 2.0f + (float)(rand() % 4);        

    unsigned char r = 255;
    unsigned char g = 150 + rand() % 80;
    unsigned char b = 0;
    unsigned char a = 150 + rand() % 105;
    f->color = (Color){ r, g, b, a };
}


void draw(Menu* this){
    if (!this) return;

    ClearBackground((Color){20, 10, 10, 255}); 

    float t = GetTime();
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    Color topColor = (Color){ (int)(40 + 20*sin(t*0.5)), 0, 0, 255 };
    Color bottomColor = (Color){ (int)(150 + 80*sin(t*0.3)), 0, 0, 255 };

    for (int y = 0; y < screenHeight; y++) {
        float factor = (float)y / (float)screenHeight;
        Color c = {
            (unsigned char)(topColor.r*(1.0f - factor) + bottomColor.r*factor),
            (unsigned char)(topColor.g*(1.0f - factor) + bottomColor.g*factor),
            (unsigned char)(topColor.b*(1.0f - factor) + bottomColor.b*factor),
            255
        };
        DrawLine(0, y, screenWidth, y, c);
    }

    if (!flamesInit) {
        for (int i = 0; i < NUM_FLAMES; i++) {
            ResetFlame(&flames[i], screenWidth, screenHeight);
            flames[i].y = (float)(rand() % screenHeight);
        }
        flamesInit = true;
    }

    float dt = GetFrameTime();
    for (int i = 0; i < NUM_FLAMES; i++) {
        FlameParticle* f = &flames[i];

        f->y -= f->speed * dt;

        if (f->y + f->radius < 0) {
            ResetFlame(f, screenWidth, screenHeight);
        }

        DrawCircle((int)f->x, (int)f->y, f->radius, f->color);
    }

    if (this->play)       this->play->draw(this->play);
    if (this->volume)     this->volume->draw(this->volume);
    if (this->difficulty) this->difficulty->draw(this->difficulty);
}



void update(Menu* this){
    if (!this) return;

    Vector2 mouse = GetMousePosition();

    Button* buttons[3] = { this->play, this->volume, this->difficulty };

    for (int i = 0; i < 3; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            if (b->action) b->action();
        }
    }
}

Menu* new_Menu(){
    Menu* this = malloc(sizeof(Menu));

    int screenWidth  = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int buttonWidth  = 300;
    int buttonHeight = 60;
    int spacing      = 20;

    int centerX = screenWidth/2  - buttonWidth/2;
    int startY  = screenHeight/2 - (buttonHeight*3 + spacing*2)/2;

    this->play = new_Button(
        centerX,
        startY,
        buttonWidth,
        buttonHeight,
        RED,       // normal color
        GREEN,     // hover color
        "PLAY",
        20,
        onPlay
    );

    this->volume = new_Button(
        centerX,
        startY + buttonHeight + spacing,
        buttonWidth,
        buttonHeight,
        (Color){ 80,  80,  80, 255 },   // normal
        (Color){120, 120, 120, 255 },   // hover
        "VOLUME",
        20,
        onVolume
    );

    this->difficulty = new_Button(
        centerX,
        startY + (buttonHeight + spacing)*2,
        buttonWidth,
        buttonHeight,
        (Color){ 80,  80,  80, 255 },   // normal
        (Color){120, 120, 120, 255 },   // hover
        "DIFFICULTY",
        20,
        onDifficulty
    );

    this->draw   = draw;
    this->update = update;
    this->free   = _free;

    return this;
}

