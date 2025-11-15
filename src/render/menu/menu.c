#include "menu.h"
#include <stdbool.h>
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

    SPRITE_CUTSCENE1,
    SPRITE_CUTSCENE2,
    SPRITE_CUTSCENE3,
    SPRITE_CUTSCENE4,

    SPRITE_COUNT
} SpritesEnum;

typedef enum {
    ANIMATION_COUNT
} AnimationsEnum;

typedef enum {
    MAIN_CONTENT,
    CUTSCENE
} MenuState;

static MenuState menuState = MAIN_CONTENT;

static FlameParticle flames[NUM_FLAMES];
static bool flamesInit = false;

static bool showTutorial = false;
static Button* volumeButtonRef = NULL;
static int volumeLevel = 2;

// ---- Aqui sao os botoes ---- 
void onPlay() {
    menuState = CUTSCENE;
}

void onTutorial() {
    showTutorial = true;
    printf("[MENU] Rapaz, clicaram no botão de tutorial\n");
}

void onVolume() {
    volumeLevel = (volumeLevel + 1) % 3;

    float vol = 1.0f;
    if (volumeLevel == 0) vol = 0.0f;
    else if (volumeLevel == 1) vol = 0.5f;
    else vol = 1.0f;

    SetMasterVolume(vol);

    if (volumeButtonRef) {
        snprintf(volumeButtonRef->text, sizeof(volumeButtonRef->text),
            "VOLUME: %d%%", (int)(vol * 100));
    }

    printf("[MENU] Volume agora em %d%%\n", (int)(vol * 100));
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

static void drawCutscene(Menu* this) {
    static const char* cutsceneTexts[] = {
        "Após perseguir Pac-Man sem sucesso pelos vários labirintos do famoso jogo clássico,\n a Gangue dos Fantasmas foi condenada a viver eternamente no Inferno de Dante.\n Para reverter essa triste e injusta punição,\n Pac-Man fez um pacto com Lúcifer visando libertar os pobres fantasmas.",

        "Enfurecido, Lúcifer só permite que os fantasmas escapem\n se conseguirem encontrar as saídas de pelo menos quatro círculos do Inferno.",

        "Além disso, ele transforma Pac-Man em Pact-Man,\n que, junto com seus clones,\n deve impedir que os fantasmas escapem.",

        "O feitiço só será quebrado se os fantasmas completarem a difícil jornada."
    };

    int idx = min(this->cutsceneIdx, 3);

    DrawSprite(this->sprites[idx + SPRITE_CUTSCENE1], 0, 0, this->width, this->height, WHITE);
    DrawRectangle(0, this->height - 210, this->width, 250, (Color) { 0, 0, 0, 150 });
    DrawText(cutsceneTexts[idx], 100, this->height - 200, 40, WHITE);
}

static void drawMainContent(Menu* this) {
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

    if (showTutorial) {
        int sw = this->width;
        int sh = this->height;

        int panelW = (int)(sw * 0.7f);
        int panelH = (int)(sh * 0.7f);
        int panelX = (sw - panelW) / 2;
        int panelY = (sh - panelH) / 2;

        DrawRectangle(panelX, panelY, panelW, panelH, (Color) { 0, 0, 0, 200 });
        DrawRectangleLines(panelX, panelY, panelW, panelH, (Color) { 255, 120, 80, 255 });

        int titleSize = 40;
        const char* title = "TUTORIAL";
        int titleWidth = MeasureText(title, titleSize);
        DrawText(title,
            panelX + (panelW - titleWidth) / 2,
            panelY + 20,
            titleSize,
            (Color) {
            255, 200, 150, 255
        });

        int textSize = 20;
        int textX = panelX + 40;
        int textY = panelY + 90;

        DrawText("MOVIMENTACAO:", textX, textY, textSize, RAYWHITE);
        textY += 30;
        DrawText("- WASD ou Setas para mover o fantasma", textX, textY, textSize, RAYWHITE);
        textY += 30;
        DrawText("- Evite o Pact-Man e seus clones", textX, textY, textSize, RAYWHITE);
        textY += 30;
        DrawText("- Colete moedas e fragmentos para progredir", textX, textY, textSize, RAYWHITE);
        textY += 40;
        DrawText("DICAS:", textX, textY, textSize, RAYWHITE);
        textY += 30;
        DrawText("- Observe o mapa e planeje sua rota", textX, textY, textSize, RAYWHITE);
        textY += 30;
        DrawText("- Fique atento aos biomas se desintegrando com o tempo", textX, textY, textSize, RAYWHITE);

        const char* backMsg = "Pressione BACKSPACE (<-) ou clique para voltar";
        int backSize = 18;
        int backWidth = MeasureText(backMsg, backSize);
        DrawText(backMsg,
            panelX + (panelW - backWidth) / 2,
            panelY + panelH - 40,
            backSize,
            (Color) {
            255, 180, 120, 255
        });

        return;
    }

    if (this->play)       this->play->draw(this->play);
    if (this->tutorial)   this->tutorial->draw(this->tutorial);
    if (this->volume)     this->volume->draw(this->volume);
    if (this->difficulty) this->difficulty->draw(this->difficulty);
}


void draw(Menu* this) {
    if (menuState == MAIN_CONTENT)
        drawMainContent(this);
    else
        drawCutscene(this);
}

static void playSound(Menu* this) {
    Audio* audio = this->audio;

    audio->updateMusic(audio, MUSIC_MENU);
}

static void updateMainContent(Menu* this) {
    Audio* audio = this->audio;
    Vector2 mouse = GetMousePosition();

    if (showTutorial) {
        if (IsKeyPressed(KEY_BACKSPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showTutorial = false;
        }
        playSound(this);
        return;
    }

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


static void updateCutscene(Menu* this) {
    playSound(this);
    if (IsKeyPressed(KEY_SPACE)) {
        this->cutsceneIdx++;

        if (this->cutsceneIdx > 3)
            state = GAME;
    }
}

void update(Menu* this) {
    if (!this) return;

    if (menuState == MAIN_CONTENT)
        updateMainContent(this);
    else
        updateCutscene(this);
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

    sprites[SPRITE_CUTSCENE1] = LoadSprite("assets/sprites/menu/cutscene1.jpg");
    sprites[SPRITE_CUTSCENE2] = LoadSprite("assets/sprites/menu/cutscene2.jpg");
    sprites[SPRITE_CUTSCENE3] = LoadSprite("assets/sprites/menu/cutscene3.jpg");
    sprites[SPRITE_CUTSCENE4] = LoadSprite("assets/sprites/menu/cutscene4.jpg");
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
        (Color) {
        255, 0, 0, 150
    },       // normal color
        (Color) {
        0, 255, 0, 150
    },     // hover color
        "PLAY",
        40,
        onPlay
    );

    this->tutorial = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        (Color) {
        80, 80, 80, 150
    },   // normal
        (Color) {
        255, 255, 255, 150
    },   // hover
        "TUTORIAL",
        40,
        onTutorial
    );

    this->volume = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        (Color) {
        80, 80, 80, 150
    },   // normal
        (Color) {
        255, 255, 255, 150
    },   // hover
        "VOLUME: 100%",
        40,
        onVolume
    );
    volumeButtonRef = this->volume;

    this->difficulty = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        (Color) {
        80, 80, 80, 150
    },   // normal
        (Color) {
        255, 255, 255, 150
    },   // hover
        "DIFFICULTY",
        40,
        onDifficulty
    );

    this->cutsceneIdx = 0;

    this->draw = draw;
    this->update = update;
    this->free = _free;

    return this;
}

