#include "credits.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//===============================================================
//  MUSICAS E SONS
//===============================================================

typedef enum {
    MUSIC_CUTSCENE1,
    MUSIC_CUTSCENE2,
    MUSIC_CUTSCENE3,

    MUSIC_SCORE,
    MUSIC_CREDITS,

    MUSIC_COUNT
} MusicEnum;

typedef enum {
    SOUND_CLICK_BUTTON,
    SOUND_COUNT
} SoundEnum;

//===============================================================
//  SPRITES E ANIMAÇÕES
//===============================================================

typedef enum {
    SPRITE_END1,
    SPRITE_END2,
    SPRITE_END3,

    SPRITE_ADD_CREDITS,
    SPRITE_CREDITS,

    SPRITE_TIME,

    SPRITE_COUNT
} SpritesEnum;

typedef enum {
    ANIMATION_COIN,
    ANIMATION_FRAGMENT,

    ANIMATION_COUNT
} AnimationsEnum;

//===============================================================
//  VARIÁVEIS INTERNAS
//===============================================================

static char buffer[1000];
static int (*cmp)(const void* a, const void* b);

//===============================================================
//  FUNÇÕES USADAS PARA O QUICKSORT
//===============================================================

static int cmpCoins(const void* a, const void* b) {
    const Score* sa = a;
    const Score* sb = b;

    if (sa->totalCoins < sb->totalCoins) return 1;
    if (sa->totalCoins > sb->totalCoins) return -1;
    return 0;
}

static int cmpFragments(const void* a, const void* b) {
    const Score* sa = a;
    const Score* sb = b;

    if (sa->totalFragments < sb->totalFragments) return 1;
    if (sa->totalFragments > sb->totalFragments) return -1;
    return 0;
}

static int cmpTime(const void* a, const void* b) {
    const Score* sa = a;
    const Score* sb = b;

    if (sa->totalTime < sb->totalTime) return -1;
    if (sa->totalTime > sb->totalTime) return 1;
    return 0;
}

//===============================================================
//  AÇÕES DOS BOTÕES
//===============================================================

static void sortCoins() {
    cmp = cmpCoins;
}

static void sortFragments() {
    cmp = cmpFragments;
}

static void sortTime() {
    cmp = cmpTime;
}

static void next() {
    state = min(state + 1, CREDITS_FINAL);
}

static void prev() {
    if (state >= CREDITS_ADD_SCORE) state = MENU_MAIN_CONTENT;
    else state = max(state - 1, CREDITS_CUTSCENE1);
}

//===============================================================
//  FUNÇÃO AUXILIAR DE ATUALIZAÇÃO DOS BOTÕES
//===============================================================

static bool updateButtons(Credits* this, Button** buttons, int length) {
    Vector2 mouse = GetMousePosition();
    Audio* audio = this->audio;

    for (int i = 0; i < length; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            if (b->action) b->action();
            return true;
        }
    }
    return false;
}

//===============================================================
//  ATUALIZAR TELAS DE CUTSCENES
//===============================================================

static void updateCutscenes(Credits* this) {
    Audio* audio = this->audio;
    int audioIdx = MUSIC_CUTSCENE1 + (state - CREDITS_CUTSCENE1);
    if (!audio->hasEndMusic(audio, audioIdx))
        audio->updateMusic(audio, audioIdx);
    else if (state < CREDITS_CUTSCENE3)
        next();

    Button* buttons[2];
    buttons[0] = this->nextBtn;
    int len = 1;

    if (state > CREDITS_CUTSCENE1) {
        strcpy(this->prevBtn->text, "BACK");
        buttons[len++] = this->prevBtn;
    }

    bool pressed = updateButtons(this, buttons, len);

    if (pressed && state <= CREDITS_CUTSCENE3) {
        audio->restartMusic(audio, MUSIC_CUTSCENE1 + (state - CREDITS_CUTSCENE1));
    } else if (state == CREDITS_ADD_SCORE) {
        this->name[0] = 0;
        this->nameIdx = 0;
    }
}

//===============================================================
//  ATUALIZAR TELA DE ADICIONAR SCORE
//===============================================================

static void updateAddScore(Credits* this) {
    Audio* audio = this->audio;
    audio->updateMusic(audio, MUSIC_SCORE);
    Button* buttons[] = { this->nextBtn };
    bool pressed = updateButtons(this, buttons, 1);

    // Cadastrar novo score
    if (pressed && state == CREDITS_SCORE) {
        Score* score = malloc(sizeof(Score));
        strcpy(score->name, this->name);
        score->totalCoins = this->score->totalCoins;
        score->totalFragments = this->score->totalFragments;
        score->totalTime = this->score->totalTime;
        this->scores->push(this->scores, score);
        return;
    }

    for (int i = 'A'; i <= 'Z'; i++) {
        if (IsKeyPressed(i)) {
            this->name[this->nameIdx++] = i;
            this->name[this->nameIdx] = 0;
            break;
        }
    }
    if (IsKeyPressed(KEY_BACKSPACE) && this->nameIdx > 0) {
        this->name[--this->nameIdx] = 0;
    }
}

//===============================================================
//  ATUALIZAR TELA DE EXIBIR SCORE
//===============================================================

static void updateShowScore(Credits* this) {
    if (this->updateCount % 8 == 0)
        for (int i = 0; i < ANIMATION_COUNT; i++)
            UpdateAnimation(&this->animations[i]);
    Audio* audio = this->audio;
    audio->updateMusic(audio, MUSIC_SCORE);
    strcpy(this->prevBtn->text, "MENU");
    Button* buttons[] = { this->prevBtn, this->nextBtn, this->sortByCoins, this->sortByFragments, this->sortByTime };
    bool pressed = updateButtons(this, buttons, 5);
    if (pressed && state == CREDITS_SCORE)
        this->scores->sort(this->scores, cmp);
}

//===============================================================
//  ATUALIZAR TELA DE CREDITOS FINAIS
//===============================================================

static void updateFinalCredits(Credits* this) {
    if (IsKeyPressed(KEY_SPACE)) {
        this->creditsMove = 0;
        state = MENU_MAIN_CONTENT;
        return;
    }

    Audio* audio = this->audio;
    audio->updateMusic(audio, MUSIC_CREDITS);

    this->creditsMove += CREDITS_SPEED;
}

//===============================================================
//  TEXTOS DAS CUTSCENES
//===============================================================

static const char* cutsceneSlide0[] = {
    "Após a árdua jornada,",
    "os fantasmas escapam da escuridão do Inferno e dos Pact-Men enfurecidos."
};
static const char* cutsceneSlide1[] = {
    "Ao cumprirem o pacto,",
    "os fantasmas finalmente alcançam a liberdade."
};
static const char* cutsceneSlide2[] = {
    "Com seu feitiço quebrado,",
    "Pac-Man retorna à forma original e celebra a restauração da paz."
};

// --- 2. Criamos um "array de arrays" para organizar os slides ---
static const char** allCutsceneSlides[] = {
    cutsceneSlide0,
    cutsceneSlide1,
    cutsceneSlide2,
};

// --- 3. Guardamos a contagem de linhas de cada slide ---
static int allSlideLineCounts[] = { 2, 2, 2 };

//===============================================================
//  DESENHAR TELA DE CUTSCENES
//===============================================================

static void drawCutscenes(Credits* this) {
    if (IsKeyPressed(KEY_SPACE)) {
        next();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        prev();
    }

    const int idx = (state - CREDITS_CUTSCENE1);

    int spriteIdx = SPRITE_END1 + idx;
    DrawSprite(this->sprites[spriteIdx], 0, 0, this->width, this->height, WHITE);
    DrawRectangle(0, this->height - 210, this->width, 250, (Color) { 0, 0, 0, 150 });

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

    if (state > CREDITS_CUTSCENE1) this->prevBtn->draw(this->prevBtn);
    this->nextBtn->draw(this->nextBtn);
}

//===============================================================
//  DESENHAR TELA DE ADICIONAR SCORE
//===============================================================

static void drawAddScore(Credits* this) {
    DrawSprite(this->sprites[SPRITE_ADD_CREDITS], 0, 0, this->width, this->height, WHITE);
    drawCenteredText("Digite seu nome:", this->width >> 1, (this->height >> 1) - 100, 70, WHITE);

    strcpy(buffer, this->name);
    strcat(buffer, (this->updateCount & 16) ? "_" : " ");
    drawCenteredText(buffer, this->width >> 1, this->height >> 1, 70, WHITE);

    this->nextBtn->draw(this->nextBtn);
}

//===============================================================
//  DESENHAR TELA DE EXIBIR SCORE
//===============================================================

static void drawShowScore(Credits* this) {
    DrawSprite(this->sprites[SPRITE_CREDITS], 0, 0, this->width, this->height, WHITE);
    ArrayList* scores = this->scores;
    const int margin = 15;
    const int size = 30;
    const int deltaIten = margin + size;
    const int space = 10;
    const int h = 4 * deltaIten + margin;
    const int delta = h + space;
    const int halfW = this->width >> 1;
    const int x = halfW - 150;

    int y = 50;
    for (int i = 0; i < scores->length; i++) {
        Score* score = scores->data[i];
        int totalSeconds = score->totalTime;
        int minuts = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        DrawRectangleRounded((Rectangle) { x, y, 300, h }, 0.5f, 16, (Color) { 0, 0, 0, 200 });
        sprintf(buffer, "%s", score->name);
        drawCenteredText(buffer, halfW, y + margin, size, RED);
        DrawAnimation(this->animations[ANIMATION_COIN], x + margin, y + margin + deltaIten, size, WHITE);
        sprintf(buffer, "%d", score->totalCoins);
        DrawText(buffer, x + size + margin * 2, y + margin + deltaIten, size, WHITE);
        DrawAnimation(this->animations[ANIMATION_FRAGMENT], x + margin, y + deltaIten * 2 + margin, size, WHITE);
        sprintf(buffer, "%d", score->totalFragments);
        DrawText(buffer, x + size + margin * 2, y + deltaIten * 2 + margin, size, WHITE);
        DrawSprite(this->sprites[SPRITE_TIME], x + margin, y + deltaIten * 3 + margin, size, size, WHITE);
        sprintf(buffer, "%02d:%02d", minuts, seconds);
        DrawText(buffer, x + size + margin * 2, y + deltaIten * 3 + margin, size, WHITE);
        y += delta;
    }

    this->nextBtn->draw(this->nextBtn);
    this->prevBtn->draw(this->prevBtn);
    this->sortByCoins->draw(this->sortByCoins);
    this->sortByFragments->draw(this->sortByFragments);
    this->sortByTime->draw(this->sortByTime);
}

//===============================================================
//  DESENHAR TELA DE CRÉDITOS FINAIS
//===============================================================

static void drawFinalCredits(Credits* this) {
    DrawRectangle(0, 0, this->width, this->height, BLACK);

    const int halfX = this->width >> 1;
    const int fontSize = 40;
    const int margin = 10;
    const int delta = fontSize + margin;

    const int startY = this->height - this->creditsMove;
    const int startIdx = max((-startY) / delta, 0); // Matemágica
    const int actualY = startY + startIdx * delta; // Matemágica
    const int endIdx = min(startIdx + 1 + (this->height - actualY) / delta, this->lines->length);

    for (int i = startIdx; i < endIdx; i++) {
        char* line = this->lines->data[i];
        int y = this->height + (i * delta) - this->creditsMove;
        drawCenteredText(line, halfX, y, fontSize, WHITE);
    }

    DrawText("Pressione barra de espaço para voltar ao menu", 50, this->height - 100, 20, GRAY);
}

//===============================================================
//  MÉTODO DE ATUALIZAÇÃO DA CLASSE
//===============================================================

static void update(Credits* this) {
    if (state <= CREDITS_CUTSCENE3 && state >= CREDITS_CUTSCENE1) updateCutscenes(this);
    else if (state == CREDITS_ADD_SCORE) updateAddScore(this);
    else if (state == CREDITS_SCORE) updateShowScore(this);
    else if (state == CREDITS_FINAL) updateFinalCredits(this);
    this->updateCount++;
}

//===============================================================
//  MÉTODO DE DESENHAR DA CLASSE
//===============================================================

static void draw(Credits* this) {
    if (state <= CREDITS_CUTSCENE3 && state >= CREDITS_CUTSCENE1) drawCutscenes(this);
    else if (state == CREDITS_ADD_SCORE) drawAddScore(this);
    else if (state == CREDITS_SCORE) drawShowScore(this);
    else if (state == CREDITS_FINAL) drawFinalCredits(this);
}

//===============================================================
//  CARREGAMENTO DE SPRITES
//===============================================================

static void loadSprites(Credits* this) {
    Sprite* sprites = this->sprites;
    Animation* animations = this->animations;

    sprites[SPRITE_END1] = LoadSprite("assets/sprites/credits/end1.jpg");
    sprites[SPRITE_END2] = LoadSprite("assets/sprites/credits/end2.jpg");
    sprites[SPRITE_END3] = LoadSprite("assets/sprites/credits/end3.jpg");

    sprites[SPRITE_ADD_CREDITS] = LoadSprite("assets/sprites/credits/add_score.jpg");
    sprites[SPRITE_CREDITS] = LoadSprite("assets/sprites/credits/score.jpg");

    sprites[SPRITE_TIME] = LoadSprite("assets/sprites/credits/clock.png");

    const char* coin[] = { "assets/sprites/itens/coin1.png", "assets/sprites/itens/coin2.png", "assets/sprites/itens/coin3.png", "assets/sprites/itens/coin4.png",
    "assets/sprites/itens/coin5.png", "assets/sprites/itens/coin6.png" };
    const char* fragment[] = { "assets/sprites/itens/newKey.png" , "assets/sprites/itens/newKey2.png", "assets/sprites/itens/newKey3.png", "assets/sprites/itens/newKey4.png",
         "assets/sprites/itens/newKey5.png", "assets/sprites/itens/newKey4.png", "assets/sprites/itens/newKey3.png", "assets/sprites/itens/newKey2.png" };

    animations[ANIMATION_COIN] = LoadAnimation(6, coin);
    animations[ANIMATION_FRAGMENT] = LoadAnimation(8, fragment);
}

//===============================================================
//  CARREGAMENTO DE ÁUDIO
//===============================================================

static void loadAudio(Credits* this) {
    Audio* audio = this->audio;

    audio->loadMusic(audio, "assets/music/end1.mp3", MUSIC_CUTSCENE1);
    audio->loadMusic(audio, "assets/music/end2.mp3", MUSIC_CUTSCENE2);
    audio->loadMusic(audio, "assets/music/end3.mp3", MUSIC_CUTSCENE3);

    audio->loadMusic(audio, "assets/music/creditos.mp3", MUSIC_CREDITS);
    audio->loadMusic(audio, "assets/music/score.mp3", MUSIC_SCORE);

    audio->loadSound(audio, "assets/sounds/click.wav", SOUND_CLICK_BUTTON);
}

//===============================================================
//  CARREGAMENTO DOS BOTÕES
//===============================================================

static void loadButtons(Credits* this) {
    int btnW = 180;
    int btnH = 70;
    int margin = 60;
    int y = this->height - btnH - margin;

    this->prevBtn = new_Button(
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
        prev
    );

    this->nextBtn = new_Button(
        this->width - btnW - margin,
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
        next
    );

    int dx = 400;
    int hx = this->width >> 1;

    this->sortByCoins = new_Button(
        hx - dx - btnW / 2,
        y,
        btnW,
        btnH,
        (Color) {
        0, 0, 255, 180
    },
        (Color) {
        255, 255, 0, 200
    },
        "MOEDAS",
        30,
        sortCoins
    );

    int bW = btnW + 70;
    this->sortByFragments = new_Button(
        hx - bW / 2,
        y,
        bW,
        btnH,
        (Color) {
        255, 0, 0, 180
    },
        (Color) {
        255, 255, 0, 200
    },
        "FRAGMENTOS",
        30,
        sortFragments
    );

    this->sortByTime = new_Button(
        hx + dx - btnW / 2,
        y,
        btnW,
        btnH,
        (Color) {
        0, 255, 0, 180
    },
        (Color) {
        255, 255, 0, 200
    },
        "TEMPO",
        30,
        sortTime
    );
}

//===============================================================
//  CARREGAMENTO DOS SCORES
//===============================================================

static void loadScores(Credits* this) {
    ArrayList* scores = this->scores;
    FILE* file = fopen("data/scores.bin", "rb");
    while (1) {
        Score* score = malloc(sizeof(Score));
        if (fread(score, sizeof(Score), 1, file) == 1) {
            scores->push(scores, score);
        } else {
            free(score);
            break;
        }
    }
    fclose(file);
}

//===============================================================
//  FUNÇÃO PARA SALVAR OS SCORES APÓS O DESCARREGAMENTO
//===============================================================

static void saveScores(Credits* this) {
    ArrayList* scores = this->scores;
    FILE* file = fopen("data/scores.bin", "wb");
    for (int i = 0; i < scores->length; i++) {
        Score* score = scores->data[i];
        fwrite(score, sizeof(Score), 1, file);
    }
    fclose(file);
}

//===============================================================
//  CARREGAMENTO DOS CRÉDITOS
//===============================================================

static void loadCredits(Credits* this) {
    ArrayList* lines = this->lines;
    FILE* file = fopen("assets/end_credits.txt", "r");
    while (1) {
        char* line = malloc(200 * sizeof(char));
        if (fgets(line, 200, file) == NULL) {
            free(line);
            break;
        } else {
            lines->push(lines, line);
        }
    }
    fclose(file);
}

//===============================================================
//  MÉTODO DE LIBERAR MEMÓRIA DA CLASSE
//===============================================================

static void _free(Credits* this) {
    this->audio->free(this->audio);
    for (int i = 0; i < ANIMATION_COUNT; i++)
        UnloadAnimation(this->animations[i]);
    free(this->animations);

    for (int i = 0; i < SPRITE_COUNT; i++)
        UnloadSprite(this->sprites[i]);
    free(this->sprites);

    this->nextBtn->free(this->nextBtn);
    this->prevBtn->free(this->prevBtn);
    this->sortByCoins->free(this->sortByCoins);
    this->sortByFragments->free(this->sortByFragments);
    this->sortByTime->free(this->sortByTime);

    for (int i = 0; i < this->lines->length; i++)
        free(this->lines->data[i]);

    this->lines->free(this->lines);

    saveScores(this);

    for (int i = 0; i < this->scores->length; i++)
        free(this->scores->data[i]);

    this->scores->free(this->scores);

    free(this);
}

//===============================================================
//  CONSTRUTOR DA CLASSE
//===============================================================

Credits* new_Credits(int width, int height, Score* score) {
    Credits* this = malloc(sizeof(Credits));

    this->width = width;
    this->height = height;

    this->name[0] = 0;
    this->nameIdx = 0;

    this->score = score;

    this->audio = new_Audio(MUSIC_COUNT, SOUND_COUNT);

    this->animations = malloc(sizeof(Animation) * ANIMATION_COUNT);
    this->sprites = malloc(sizeof(Sprite) * SPRITE_COUNT);

    loadSprites(this);
    loadButtons(this);
    loadAudio(this);

    this->scores = new_ArrayList();
    loadScores(this);

    this->lines = new_ArrayList();
    loadCredits(this);

    this->updateCount = 0;
    this->creditsMove = 0;

    this->update = update;
    this->draw = draw;
    this->free = _free;

    return this;
}