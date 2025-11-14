#include "menu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h" 
#include <raylib.h> 
#include <math.h>

typedef enum {
    MUSIC_MENU,

    MUSIC_COUNT
} MusicEnum;

typedef enum {
    SOUND_CLICK_BUTTON,
    SOUND_COUNT
} SoundEnum;

typedef enum {
    SPRITE_BACKGROUND,

    SPRITE_COUNT
} SpritesEnum;

typedef enum {
    ANIMATION_COUNT
} AnimationsEnum;

static FlameParticle flames[NUM_FLAMES];
static bool flamesInit = false;

// ---- Aqui sao os botoes ---- 
void onPlay() {
    state = GAME;
}

void onTutorial() {
    printf("[MENU] Rapaz, clicaram no botão de tutorial\n");
}

void onVolume() {
    printf("[MENU] Rapaz, clicaram no botão de volume\n");
}

void onDifficulty() {
    printf("[MENU] Rapaz, clicaram no botão de dificuldade\n");
}

// ---- Metodos do menu viu galera ---- :D
void _free(Menu* this) {
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

    for (int i = 0; i < ANIMATION_COUNT; i++) {
        UnloadAnimation(this->animations[i]);
    }

    free(this->animations);

    for (int i = 0; i < SPRITE_COUNT; i++) {
        UnloadSprite(this->sprites[i]);
    }

    free(this->sprites);

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


void draw(Menu* this) {
    if (!this) return;

    //float t = GetTime();

    // Color topColor = (Color){ (int)(40 + 20 * sin(t * 0.5)), 0, 0, 255 };
    // Color bottomColor = (Color){ (int)(150 + 80 * sin(t * 0.3)), 0, 0, 255 };

    // for (int y = 0; y < this->height; y++) {
    //     float factor = (float)y / (float)this->height;
    //     Color c = {
    //         (unsigned char)(topColor.r * (1.0f - factor) + bottomColor.r * factor),
    //         (unsigned char)(topColor.g * (1.0f - factor) + bottomColor.g * factor),
    //         (unsigned char)(topColor.b * (1.0f - factor) + bottomColor.b * factor),
    //         255
    //     };
    //     DrawLine(0, y, this->width, y, c);
    // }

    DrawSprite(this->sprites[SPRITE_BACKGROUND], 0, 0, this->width, this->height, WHITE);

    if (!flamesInit) {
        for (int i = 0; i < NUM_FLAMES; i++) {
            ResetFlame(&flames[i], this->width, this->height);
            flames[i].y = (float)(rand() % this->height);
        }
        flamesInit = true;
    }

    float dt = GetFrameTime();
    for (int i = 0; i < NUM_FLAMES; i++) {
        FlameParticle* f = &flames[i];

        f->y -= f->speed * dt;

        if (f->y + f->radius < 0) {
            ResetFlame(f, this->width, this->height);
        }

        DrawCircle((int)f->x, (int)f->y, f->radius, f->color);
    }

    if (this->play)       this->play->draw(this->play);
    if (this->tutorial)   this->tutorial->draw(this->tutorial);
    if (this->volume)     this->volume->draw(this->volume);
    if (this->difficulty) this->difficulty->draw(this->difficulty);
}

static void playSound(Menu* this) {
    Audio* audio = this->audio;

    audio->updateMusic(audio, MUSIC_MENU);
}

void update(Menu* this) {
    if (!this) return;

    Audio* audio = this->audio;
    Vector2 mouse = GetMousePosition();

    Button* buttons[4] = { this->play, this->tutorial, this->volume, this->difficulty };

    for (int i = 0; i < 4; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            if (b->action) b->action();
        }
    }

    playSound(this);
}

static void loadAudio(Menu* this) {
    Audio* audio = new_Audio(MUSIC_COUNT, SOUND_COUNT);
    this->audio = audio;

    audio->loadMusic(audio, "assets/music/menu_music.mp3", MUSIC_MENU);
    audio->loadSound(audio, "assets/sounds/fragmento.wav", SOUND_CLICK_BUTTON);
}

static void loadSprites(Menu* this) {
    //Animation* animations = this->animations;
    Sprite* sprites = this->sprites;

    sprites[SPRITE_BACKGROUND] = LoadSprite("assets/sprites/menu/menu.jpg");
}

Menu* new_Menu(int width, int height) {
    Menu* this = malloc(sizeof(Menu));

    this->width = width;
    this->height = height;

    int buttonWidth = 300;
    int buttonHeight = 60;
    int spacing = 20;

    int delta = buttonHeight + spacing;

    int centerX = width / 2 - buttonWidth / 2;
    int startY = height / 2 - (buttonHeight * 3 + spacing * 2) / 2;

    this->animations = malloc(sizeof(Animation) * ANIMATION_COUNT);
    this->sprites = malloc(sizeof(Sprite) * SPRITE_COUNT);

    loadAudio(this);
    loadSprites(this);

    this->play = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        RED,       // normal color
        GREEN,     // hover color
        "PLAY",
        40,
        onPlay
    );

    this->tutorial = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        (Color) {80, 80, 80, 255},   // normal
        (Color) {120, 120, 120, 255},   // hover
        "TUTORIAL",
        40,
        onTutorial
    );

    this->volume = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        (Color) {80, 80, 80, 255},   // normal
        (Color) {120, 120, 120, 255},   // hover
        "VOLUME",
        40,
        onVolume
    );

    this->difficulty = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        (Color) {80, 80, 80, 255},   // normal
        (Color) {120, 120, 120, 255},   // hover
        "DIFFICULTY",
        40,
        onDifficulty
    );

    this->draw = draw;
    this->update = update;
    this->free = _free;

    return this;
}

