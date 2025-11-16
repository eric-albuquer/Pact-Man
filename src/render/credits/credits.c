#include "credits.h"
#include <stdlib.h>

typedef enum {
    MUSIC_CUTSCENE1,
    MUSIC_CUTSCENE2,

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

    SPRITE_COUNT
} SpritesEnum;

typedef enum {
    ANIMATION_COUNT
} AnimationsEnum;

typedef enum {
    CUTSCENE1,
    CUTSCENE2,

    FINAL_CREDITS
} CreditsState;

CreditsState creditsState = CUTSCENE1;

static void next() {
    creditsState = min(creditsState + 1, FINAL_CREDITS);
}

static void prev() {
    creditsState = max(creditsState - 1, CUTSCENE1);
}

static void updateCutscenes(Credits* this) {
    Vector2 mouse = GetMousePosition();
    Audio* audio = this->audio;

    if (!audio->hasEndMusic(audio, MUSIC_CUTSCENE1 + creditsState))
        audio->updateMusic(audio, MUSIC_CUTSCENE1 + creditsState);

    Button* buttons[2] = { this->cutscenePrev, this->cutsceneNext };

    for (int i = 0; i < 2; i++) {
        Button* b = buttons[i];
        if (!b) continue;

        b->hovered = b->isInside(b, mouse);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && b->hovered) {
            audio->playSound(audio, SOUND_CLICK_BUTTON);
            if (b->action) b->action();
            if (creditsState < FINAL_CREDITS)
                audio->restartMusic(audio, MUSIC_CUTSCENE1 + creditsState);
        }
    }
}

static void updateFinalCredits(Credits* this) {

}

static void update(Credits* this) {
    if (creditsState < FINAL_CREDITS) updateCutscenes(this);
    else updateFinalCredits(this);
}

static void drawCutscenes(Credits* this) {
    static const char* cutsceneTexts[] = {
            "Após a árdua jornada e a superação dos quatro círculos do Inferno,\n os fantasmas finalmente alcançam a liberdade,\n deixando para trás a escuridão e os Pact-Men enfurecidos.\n O pacto é cumprido!",

            "Com o feitiço quebrado, Pac-Man retorna à sua forma original,\n celebrando a liberdade de seus amigos e a restauração da paz,\n mesmo que em um paraíso que nunca imaginaram."
    };

    DrawSprite(this->sprites[SPRITE_END1 + creditsState], 0, 0, this->width, this->height, WHITE);
    DrawRectangle(0, this->height - 210, this->width, 250, (Color) { 0, 0, 0, 150 });
    DrawText(cutsceneTexts[creditsState], 100, this->height - 200, 40, WHITE);

    if (creditsState > CUTSCENE1) this->cutscenePrev->draw(this->cutscenePrev);
    this->cutsceneNext->draw(this->cutsceneNext);
}

static void drawFinalCredits(Credits* this) {
    DrawRectangle(0, 0, this->width, this->height, BLACK);
}

static void draw(Credits* this) {
    if (creditsState < FINAL_CREDITS) drawCutscenes(this);
    else drawFinalCredits(this);
}

static void loadSprites(Credits* this) {
    Sprite* sprites = this->sprites;
    //Animation* animations = this->animations;

    sprites[SPRITE_END1] = LoadSprite("assets/sprites/credits/end1.jpg");
    sprites[SPRITE_END2] = LoadSprite("assets/sprites/credits/end2.jpg");
}

static void loadAudio(Credits* this) {
    Audio* audio = this->audio;

    audio->loadMusic(audio, "assets/music/cutscene1.mp3", MUSIC_CUTSCENE1);
    audio->loadMusic(audio, "assets/music/cutscene2.mp3", MUSIC_CUTSCENE2);

    audio->loadSound(audio, "assets/sounds/click.wav", SOUND_CLICK_BUTTON);
}

static void loadButtons(Credits* this) {
    int btnW = 160;
    int btnH = 50;
    int margin = 40;
    int y = this->height - btnH - margin;
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
        prev
    );

    this->cutsceneNext = new_Button(
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
}

static void _free(Credits* this) {
    this->audio->free(this->audio);
    for (int i = 0; i < ANIMATION_COUNT; i++)
        UnloadAnimation(this->animations[i]);
    free(this->animations);

    for (int i = 0; i < SPRITE_COUNT; i++)
        UnloadSprite(this->sprites[i]);
    free(this->sprites);

    free(this);
}

Credits* new_Credits(int width, int height) {
    Credits* this = malloc(sizeof(Credits));

    this->width = width;
    this->height = height;

    this->audio = new_Audio(MUSIC_COUNT, SOUND_COUNT);

    this->animations = malloc(sizeof(Animation) * ANIMATION_COUNT);
    this->sprites = malloc(sizeof(Sprite) * SPRITE_COUNT);

    loadSprites(this);
    loadButtons(this);
    loadAudio(this);

    this->update = update;
    this->draw = draw;
    this->free = _free;

    return this;
}