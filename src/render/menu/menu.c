#include "menu.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "common.h" 
#include <raylib.h> 
#include <math.h>
#include <string.h>

typedef enum {
    MUSIC_MENU,

    MUSIC_CUTSCENE1,
    MUSIC_CUTSCENE2,
    MUSIC_CUTSCENE3,
    MUSIC_CUTSCENE4,
    MUSIC_CUTSCENE5,

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
    SPRITE_CUTSCENE5,

    SPRITE_COUNT
} SpritesEnum;

typedef enum {
    ANIMATION_COUNT
} AnimationsEnum;

static Menu* currentMenu = NULL;

static FlameParticle flames[NUM_FLAMES];
static bool flamesInit = false;

static bool showTutorial = false;
static Button* volumeButtonRef = NULL;
static int volumeLevel = 2;

// ---- Aqui sao os botoes ---- 
void onPlay() {
    state = MENU_CUTSCENE1;
}

void onTutorial() {
    showTutorial = true;
    printf("[MENU] Rapaz, clicaram no botão de tutorial\n");
}

void onVolume() {
    volumeLevel = (volumeLevel + 1) % 101;

    SetMasterVolume(volumeLevel / 100.0f);

    if (volumeButtonRef) {
        snprintf(volumeButtonRef->text, sizeof(volumeButtonRef->text),
            "VOLUME: %d%%", volumeLevel);
    }

    printf("[MENU] Volume agora em %d%%\n", volumeLevel);
}

void onDifficulty() {
    printf("[MENU] Rapaz, clicaram no botão de dificuldade\n");
}

void onCutsceneNext() {
    if (!currentMenu) return;
    if (state < MENU_CUTSCENE1 || state > MENU_CUTSCENE5) return;

    if (state < MENU_CUTSCENE5) {
        state++;
    } else {
        state = GAME_MAIN_CONTENT;
    }
}

void onCutscenePrev() {
    if (!currentMenu) return;
    if (state < MENU_CUTSCENE1 || state > MENU_CUTSCENE5) return;

    if (state > MENU_CUTSCENE1) {
        state--;
    } else
        state = MENU_MAIN_CONTENT;
}

void onCredits() {
    state = CREDITS_FINAL;
}

void onScore() {
    state = CREDITS_SCORE;
}

// ---- Metodos do menu viu galera ---- :D
void _free(Menu* this) {
    if (!this) return;

    if (this->play) {
        this->play->free(this->play);
    }
    if (this->tutorial) {
        this->tutorial->free(this->tutorial);
    }
    if (this->volume) {
        this->volume->free(this->volume);
    }
    if (this->difficulty) {
        this->difficulty->free(this->difficulty);
    }
    if (this->cutscenePrev) {
        this->cutscenePrev->free(this->cutscenePrev);
    }
    if (this->cutsceneNext) {
        this->cutsceneNext->free(this->cutsceneNext);
    }

    // for (int i = 0; i < ANIMATION_COUNT; i++) {
    //     UnloadAnimation(this->animations[i]);
    // }
    // free(this->animations);

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

static const char* cutsceneSlide0[] = {
    "Após perseguir Pac-Man sem sucesso pelos vários labirintos do famoso jogo clássico,",
    "a Gangue dos Fantasmas foi condenada a viver eternamente no Inferno de Dante."
};
static const char* cutsceneSlide1[] = {
    "Para reverter essa triste e injusta punição,",
    "Pac-Man fez um pacto com Lúcifer visando libertar os pobres fantasmas."
};
static const char* cutsceneSlide2[] = {
    "Enfurecido, Lúcifer só permite que os fantasmas escapem",
    "se conseguirem encontrar as saídas de pelo menos quatro círculos do Inferno."
};
static const char* cutsceneSlide3[] = {
    "Além disso, ele transforma Pac-Man em Pact-Man,",
    "que, junto com seus clones,",
    "deve impedir que os fantasmas escapem."
};
static const char* cutsceneSlide4[] = {
    "O feitiço só será quebrado se os fantasmas completarem a difícil jornada."
};

// --- 2. Criamos um "array de arrays" para organizar os slides ---
static const char** allCutsceneSlides[] = {
    cutsceneSlide0,
    cutsceneSlide1,
    cutsceneSlide2,
    cutsceneSlide3,
    cutsceneSlide4
};

// --- 3. Guardamos a contagem de linhas de cada slide ---
static int allSlideLineCounts[] = { 2, 2, 2, 3, 1 };


// --- 4. Sua função 'drawCutscene' ajustada ---
static void drawCutscene(Menu* this) {
    // Esta parte fica igual
    int idx = min(state, MENU_CUTSCENE5) - MENU_CUTSCENE1;

    DrawSprite(this->sprites[idx + SPRITE_CUTSCENE1], 0, 0, this->width, this->height, WHITE);
    DrawRectangle(0, this->height - 210, this->width, 250, (Color) { 0, 0, 0, 150 });

    // --- Início do Bloco de Centralização Simples ---

    // Removemos esta linha:
    // DrawText(cutsceneTexts[idx], 100, this->height - 200, 40, WHITE);

    // E adicionamos este loop simples:

    const char** currentSlideLines = allCutsceneSlides[idx]; // Pega o slide certo
    int lineCount = allSlideLineCounts[idx];           // Pega o n° de linhas

    int fontSize = 40;
    int lineSpacing = 10;
    int posY = this->height - 200; // Posição Y inicial

    for (int i = 0; i < lineCount; i++) {
        const char* line = currentSlideLines[i];

        // 1. Mede o tamanho da linha
        int lineWidth = MeasureText(line, fontSize);

        // 2. Calcula o X centralizado
        int posX = (this->width - lineWidth) / 2;

        // 3. Desenha a linha
        DrawText(line, posX, posY, fontSize, WHITE);

        // 4. Move o Y para a próxima linha
        posY += fontSize + lineSpacing;
    }
    // --- Fim do Bloco de Centralização ---

    // Esta parte final fica igual
    if (this->cutscenePrev) this->cutscenePrev->draw(this->cutscenePrev);
    if (state < MENU_CUTSCENE5) {
        strcpy(this->cutsceneNext->text, "NEXT");
        this->cutsceneNext->color = (Color){ 0, 0, 0, 180 };
        this->cutsceneNext->fontColor = (Color){ 255, 255, 255, 150 };
    } else {
        strcpy(this->cutsceneNext->text, "JOGAR");
        this->cutsceneNext->color = (Color){ 0, 255, 0, 255 };
        this->cutsceneNext->fontColor = (Color){ 255, 0, 0, 255 };
    }
    if (this->cutsceneNext) this->cutsceneNext->draw(this->cutsceneNext);
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
    if (this->credits)    this->credits->draw(this->credits);
    if (this->score)      this->score->draw(this->score);
}


void draw(Menu* this) {
    if (state == MENU_MAIN_CONTENT)
        drawMainContent(this);
    else if (state >= MENU_CUTSCENE1 && state <= MENU_CUTSCENE5)
        drawCutscene(this);
}

static void playSound(Menu* this) {
    Audio* audio = this->audio;

    audio->updateMusic(audio, MUSIC_MENU);

    int cutsceneIdx = state - MENU_CUTSCENE1;

    if (state >= MENU_CUTSCENE1) {
        if (!audio->hasEndMusic(audio, MUSIC_CUTSCENE1 + cutsceneIdx))
            audio->updateMusic(audio, MUSIC_CUTSCENE1 + cutsceneIdx);
        else
            state = min(state + 1, MENU_CUTSCENE5);
    }
}

static void updateMainContent(Menu* this) {
    Audio* audio = this->audio;
    Vector2 mouse = GetMousePosition();

    playSound(this);

    if (showTutorial) {
        if (IsKeyPressed(KEY_BACKSPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showTutorial = false;
        }
        return;
    }

    Button* buttons[6] = { this->play, this->tutorial, this->volume, this->difficulty, this->credits, this->score };

    for (int i = 0; i < 6; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            if (b->action) b->action();
        }
    }
}


static void updateCutscene(Menu* this) {
    playSound(this);

    if (IsKeyPressed(KEY_SPACE)) {
        onCutsceneNext();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        onCutscenePrev();
    }

    Vector2 mouse = GetMousePosition();
    Audio* audio = this->audio;

    Button* buttons[2] = { this->cutscenePrev, this->cutsceneNext };

    for (int i = 0; i < 2; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            if (b->action) b->action();
            if (state <= MENU_CUTSCENE5)
                audio->restartMusic(audio, MUSIC_CUTSCENE1 + state - MENU_CUTSCENE1);
        }
    }
}

void update(Menu* this) {
    if (!this) return;

    if (state == MENU_MAIN_CONTENT)
        updateMainContent(this);
    else if (state >= MENU_CUTSCENE1 && state <= MENU_CUTSCENE5)
        updateCutscene(this);
}

static void loadAudio(Menu* this) {
    Audio* audio = new_Audio(MUSIC_COUNT, SOUND_COUNT);
    this->audio = audio;

    audio->loadMusic(audio, "assets/music/menu_music.mp3", MUSIC_MENU);

    audio->loadMusic(audio, "assets/music/cutscene1.mp3", MUSIC_CUTSCENE1);
    audio->loadMusic(audio, "assets/music/cutscene2.mp3", MUSIC_CUTSCENE2);
    audio->loadMusic(audio, "assets/music/cutscene3.mp3", MUSIC_CUTSCENE3);
    audio->loadMusic(audio, "assets/music/cutscene4.mp3", MUSIC_CUTSCENE4);
    audio->loadMusic(audio, "assets/music/cutscene5.mp3", MUSIC_CUTSCENE5);

    audio->loadSound(audio, "assets/sounds/click.wav", SOUND_CLICK_BUTTON);
}

static void loadSprites(Menu* this) {
    //Animation* animations = this->animations;
    Sprite* sprites = this->sprites;

    sprites[SPRITE_BACKGROUND] = LoadSprite("assets/sprites/menu/menu.jpg");

    sprites[SPRITE_CUTSCENE1] = LoadSprite("assets/sprites/menu/cutscene1.jpg");
    sprites[SPRITE_CUTSCENE2] = LoadSprite("assets/sprites/menu/cutscene2.jpg");
    sprites[SPRITE_CUTSCENE3] = LoadSprite("assets/sprites/menu/cutscene3.jpg");
    sprites[SPRITE_CUTSCENE4] = LoadSprite("assets/sprites/menu/cutscene4.jpg");
    sprites[SPRITE_CUTSCENE5] = LoadSprite("assets/sprites/menu/cutscene5.jpg");
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
    int startY = height / 2 - (buttonHeight * 5 + spacing * 2) / 2;

    //this->animations = malloc(sizeof(Animation) * ANIMATION_COUNT);
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
    },
        (Color) {
        255, 255, 255, 150
    },
        "DIFFICULTY",
        40,
        onDifficulty
    );

    this->credits = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        (Color) {
        80, 80, 80, 150
    },
        (Color) {
        255, 255, 255, 150
    },
        "CRÉDITOS",
        40,
        onCredits
    );

    this->score = new_Button(
        centerX,
        startY += delta,
        buttonWidth,
        buttonHeight,
        (Color) {
        80, 80, 80, 150
    },
        (Color) {
        255, 255, 255, 150
    },
        "SCORE",
        40,
        onScore
    );

    int btnW = 160;
    int btnH = 50;
    int margin = 40;
    int y = height - btnH - margin;

    this->cutscenePrev = new_Button(
        margin,
        y,
        btnW,
        btnH,
        (Color) {
        0, 0, 0, 180
    },
        (Color) {
        255, 255, 255, 200
    },
        "< BACK",
        30,
        onCutscenePrev
    );

    this->cutsceneNext = new_Button(
        width - btnW - margin,
        y,
        btnW,
        btnH,
        (Color) {
        0, 0, 0, 180
    },
        (Color) {
        255, 255, 255, 200
    },
        "NEXT >",
        30,
        onCutsceneNext
    );

    this->draw = draw;
    this->update = update;
    this->free = _free;

    currentMenu = this;

    return this;
}


