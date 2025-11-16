#include "credits.h"
#include <stdlib.h>
#include <string.h>

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

typedef enum {
    SPRITE_END1,
    SPRITE_END2,
    SPRITE_END3,

    SPRITE_ADD_CREDITS,
    SPRITE_CREDITS,

    SPRITE_COUNT
} SpritesEnum;

typedef enum {
    ANIMATION_COUNT
} AnimationsEnum;

typedef struct {
    int y;
    char text[200];
} Line;

typedef struct {
    char name[100];
    int totalCoins;
    int totalFragments;
    float totalTime;
} Score;

static ArrayList* thisScores;

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

static void sortCoins() {
    thisScores->sort(thisScores, cmpCoins);
}

static void sortFragments() {
    thisScores->sort(thisScores, cmpFragments);
}

static void sortTime() {
    thisScores->sort(thisScores, cmpTime);
}

static void next() {
    state = min(state + 1, CREDITS_FINAL);
}

static void prev() {
    if (state >= CREDITS_ADD_SCORE) state = MENU_MAIN_CONTENT;
    else state = max(state - 1, CREDITS_CUTSCENE1);
}

static void updateCutsceneButtons(Credits* this) {
    Vector2 mouse = GetMousePosition();
    Audio* audio = this->audio;

    int audioIdx = MUSIC_CUTSCENE1 + (state - CREDITS_CUTSCENE1);
    if (!audio->hasEndMusic(audio, audioIdx))
        audio->updateMusic(audio, audioIdx);

    strcpy(this->prevBtn->text, "BACK");

    Button* buttons[2] = { this->prevBtn, this->nextBtn };

    for (int i = 0; i < 2; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            if (b->action) b->action();
            if (state <= CREDITS_CUTSCENE3)
                audio->restartMusic(audio, MUSIC_CUTSCENE1 + (state - CREDITS_CUTSCENE1));
        }
    }
}

static void updateScoreButton(Credits* this) {
    Vector2 mouse = GetMousePosition();
    Audio* audio = this->audio;

    Button* buttons[2] = { this->prevBtn, this->nextBtn };

    strcpy(this->prevBtn->text, "MENU");

    for (int i = 0; i < 2; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            if (b->action) b->action();
            if (state == CREDITS_SCORE) {
                Score* score = malloc(sizeof(Score));
                strcpy(score->name, this->name);
                score->totalCoins = this->player->totalCoins;
                score->totalFragments = this->player->totalFragment;
                score->totalTime = this->player->totalTime;
                this->scores->push(this->scores, score);
            }
        }
    }
}

static void updateSortButtons(Credits* this) {
    Vector2 mouse = GetMousePosition();
    Audio* audio = this->audio;

    Button* buttons[3] = { this->sortByCoins, this->sortByFragments, this->sortByTime };

    for (int i = 0; i < 3; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            if (b->action) b->action();
        }
    }
}

static void updateCutscenes(Credits* this) {
    updateCutsceneButtons(this);
}

static void updateScore(Credits* this) {
    Audio* audio = this->audio;
    audio->updateMusic(audio, MUSIC_SCORE);
    updateScoreButton(this);
    updateSortButtons(this);
}

static void updateAddScore(Credits* this) {
    Audio* audio = this->audio;
    audio->updateMusic(audio, MUSIC_SCORE);
    updateScoreButton(this);

    for (int i = 'A'; i <= 'Z'; i++) {
        if (IsKeyPressed(i)) {
            this->name[this->nameIdx++] = i;
            this->name[this->nameIdx] = 0;
            break;
        }
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        this->name[--this->nameIdx] = 0;
    }

    printf("%s\n", this->name);
}

static void clearLines(Credits* this) {
    LinkedList* lines = this->lines;
    while (lines->length > 0) {
        Line* line = lines->removeFirst(lines);
        free(line);
    }
}

static void resetFinalCredits(Credits* this) {
    clearLines(this);
    rewind(this->file);
}

static void updateFinalCredits(Credits* this) {
    if (IsKeyPressed(KEY_SPACE)) {
        resetFinalCredits(this);
        state = MENU_MAIN_CONTENT;
        return;
    }

    Audio* audio = this->audio;
    audio->updateMusic(audio, MUSIC_CREDITS);

    LinkedList* lines = this->lines;

    if (this->updateCount % 40 == 0 && !this->creditsEnd) {
        Line* line = malloc(sizeof(Line));
        line->y = this->height;
        if (fgets(line->text, sizeof(line->text), this->file) == NULL) {
            this->creditsEnd = true;
            free(line);
        } else {
            lines->addLast(lines, line);
        }
    }

    Node* node = lines->head;
    if (node != NULL) {
        Line* line = node->data;
        if (line->y < -40) {
            lines->removeFirst(lines);
            free(line);
        }
    }

    while (node != NULL) {
        Line* line = node->data;
        line->y--;
        node = node->next;
    }
}

static void drawCutscenes(Credits* this) {
    if (IsKeyPressed(KEY_SPACE)) {
        next();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        prev();
    }

    static const char* cutsceneTexts[] = {
            "Após a árdua jornada,\n os fantasmas escapam da escuridão do Inferno e dos Pact-Men enfurecidos.",

            "Ao cumprirem o pacto,\n os fantasmas finalmente alcançam a liberdade.",

            "Com seu feitiço quebrado,\nPac-Man retorna à forma original e celebra a restauração da paz."
    };

    int spriteIdx = SPRITE_END1 + (state - CREDITS_CUTSCENE1);
    DrawSprite(this->sprites[spriteIdx], 0, 0, this->width, this->height, WHITE);
    DrawRectangle(0, this->height - 210, this->width, 250, (Color) { 0, 0, 0, 150 });
    drawCenteredText(cutsceneTexts[(state - CREDITS_CUTSCENE1)], this->width >> 1, this->height - 200, 40, WHITE);

    if (state > CREDITS_CUTSCENE1) this->prevBtn->draw(this->prevBtn);
    this->nextBtn->draw(this->nextBtn);
}

static void drawFinalCredits(Credits* this) {
    DrawRectangle(0, 0, this->width, this->height, BLACK);

    LinkedList* lines = this->lines;
    Node* node = lines->head;

    const int halfX = this->width >> 1;

    while (node != NULL) {
        Line* line = node->data;
        drawCenteredText(line->text, halfX, line->y, 40, WHITE);
        node = node->next;
    }

    DrawText("Pressione barra de espaço para voltar ao menu", 50, this->height - 100, 20, GRAY);
}

static void drawAddScore(Credits* this) {
    DrawSprite(this->sprites[SPRITE_ADD_CREDITS], 0, 0, this->width, this->height, WHITE);
    drawCenteredText("Digite seu nome:", this->width >> 1, (this->height >> 1) - 100, 40, BLACK);
    static char buffer[1000];
    strcpy(buffer, this->name);
    strcat(buffer, (this->updateCount & 16) ? "_" : " ");
    drawCenteredText(buffer, this->width >> 1, this->height >> 1, 40, BLACK);

    this->nextBtn->draw(this->nextBtn);
    this->prevBtn->draw(this->prevBtn);
}

static void drawScore(Credits* this) {
    DrawSprite(this->sprites[SPRITE_CREDITS], 0, 0, this->width, this->height, WHITE);
    ArrayList* scores = this->scores;
    static char buffer[1000];
    int delta = 100;
    int y = 100;
    int halfW = this->width >> 1;
    for (int i = 0; i < scores->length; i++) {
        Score* score = scores->data[i];
        sprintf(buffer, "Name: %s\nTotalCoins: %d\nTotalFragmens: %d\nTotalTime: %.2f", score->name, score->totalCoins, score->totalFragments, score->totalTime);
        DrawRectangle(halfW - 100, y - 5, 200, 90, (Color){0, 0, 0, 200});
        drawCenteredText(buffer, halfW, y, 20, WHITE);
        y += delta;
        
    }

    this->nextBtn->draw(this->nextBtn);
    this->prevBtn->draw(this->prevBtn);
    this->sortByCoins->draw(this->sortByCoins);
    this->sortByFragments->draw(this->sortByFragments);
    this->sortByTime->draw(this->sortByTime);
}

static void update(Credits* this) {
    if (state <= CREDITS_CUTSCENE3 && state >= CREDITS_CUTSCENE1) updateCutscenes(this);
    else if (state == CREDITS_ADD_SCORE) updateAddScore(this);
    else if (state == CREDITS_SCORE) updateScore(this);
    else if (state == CREDITS_FINAL) updateFinalCredits(this);
    this->updateCount++;
}

static void draw(Credits* this) {
    if (state <= CREDITS_CUTSCENE3 && state >= CREDITS_CUTSCENE1) drawCutscenes(this);
    else if (state == CREDITS_ADD_SCORE) drawAddScore(this);
    else if (state == CREDITS_SCORE) drawScore(this);
    else if (state == CREDITS_FINAL) drawFinalCredits(this);
}

static void loadSprites(Credits* this) {
    Sprite* sprites = this->sprites;
    //Animation* animations = this->animations;

    sprites[SPRITE_END1] = LoadSprite("assets/sprites/credits/end1.jpg");
    sprites[SPRITE_END2] = LoadSprite("assets/sprites/credits/end2.jpg");
    sprites[SPRITE_END3] = LoadSprite("assets/sprites/credits/end3.jpg");

    sprites[SPRITE_ADD_CREDITS] = LoadSprite("assets/sprites/credits/add_score.jpg");
    sprites[SPRITE_CREDITS] = LoadSprite("assets/sprites/credits/score.jpg");
}

static void loadAudio(Credits* this) {
    Audio* audio = this->audio;

    audio->loadMusic(audio, "assets/music/cutscene1.mp3", MUSIC_CUTSCENE1);
    audio->loadMusic(audio, "assets/music/cutscene2.mp3", MUSIC_CUTSCENE2);
    audio->loadMusic(audio, "assets/music/cutscene2.mp3", MUSIC_CUTSCENE3);
    audio->loadMusic(audio, "assets/music/credits.mp3", MUSIC_CREDITS);
    audio->loadMusic(audio, "assets/music/score.mp3", MUSIC_SCORE);

    audio->loadSound(audio, "assets/sounds/click.wav", SOUND_CLICK_BUTTON);
}

static void loadButtons(Credits* this) {
    int btnW = 160;
    int btnH = 50;
    int margin = 40;
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

    int dx = 300;
    int hx = this->width >> 1;

    this->sortByCoins = new_Button(
        hx - dx - btnW / 2,
        y,
        btnW,
        btnH,
        (Color) {
        0, 0, 0, 180
    },
        (Color) {
        255, 255, 255, 200
    },
        "MOEDAS",
        30,
        sortCoins
    );

    this->sortByFragments = new_Button(
        hx - btnW / 2,
        y,
        btnW + 70,
        btnH,
        (Color) {
        0, 0, 0, 180
    },
        (Color) {
        255, 255, 255, 200
    },
        "FRAGMENTOS",
        30,
        sortFragments
    );

    this->sortByTime = new_Button(
        hx + dx,
        y,
        btnW,
        btnH,
        (Color) {
        0, 0, 0, 180
    },
        (Color) {
        255, 255, 255, 200
    },
        "TEMPO",
        30,
        sortTime
    );
}

static void loadScores(Credits* this) {
    this->scores = new_ArrayList();
    thisScores = this->scores;
    ArrayList* scores = this->scores;
    FILE* file = fopen("scores.bin", "rb");
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

static void saveScores(Credits* this) {
    ArrayList* scores = this->scores;
    FILE* file = fopen("scores.bin", "wb");
    for (int i = 0; i < scores->length; i++) {
        Score* score = scores->data[i];
        fwrite(score, sizeof(Score), 1, file);
    }
    fclose(file);
}

static void _free(Credits* this) {
    this->audio->free(this->audio);
    for (int i = 0; i < ANIMATION_COUNT; i++)
        UnloadAnimation(this->animations[i]);
    free(this->animations);

    for (int i = 0; i < SPRITE_COUNT; i++)
        UnloadSprite(this->sprites[i]);
    free(this->sprites);

    fclose(this->file);

    this->nextBtn->free(this->nextBtn);
    this->prevBtn->free(this->prevBtn);
    this->sortByCoins->free(this->sortByCoins);
    this->sortByFragments->free(this->sortByFragments);
    this->sortByTime->free(this->sortByTime);

    clearLines(this);

    this->lines->free(this->lines);

    saveScores(this);

    for (int i = 0; i < this->scores->length; i++) {
        Score* score = this->scores->data[i];
        free(score);
    }

    this->scores->free(this->scores);

    free(this);
}

Credits* new_Credits(int width, int height, Player* player) {
    Credits* this = malloc(sizeof(Credits));

    this->width = width;
    this->height = height;

    this->name[0] = 0;
    this->nameIdx = 0;

    this->player = player;

    this->audio = new_Audio(MUSIC_COUNT, SOUND_COUNT);

    this->animations = malloc(sizeof(Animation) * ANIMATION_COUNT);
    this->sprites = malloc(sizeof(Sprite) * SPRITE_COUNT);

    loadSprites(this);
    loadButtons(this);
    loadAudio(this);

    loadScores(this);

    this->updateCount = 0;
    this->creditsEnd = false;

    this->lines = new_LinkedList();
    this->file = fopen("assets/end_credits.txt", "r");

    this->update = update;
    this->draw = draw;
    this->free = _free;

    return this;
}