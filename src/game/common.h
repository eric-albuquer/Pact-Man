#ifndef COMMON_H
#define COMMON_H

#include <raylib.h>

#define HUD_OPACITY (Color){ 0, 0, 0, 200 }

typedef enum {
    LUXURIA,
    GULA,
    HERESIA,
    VIOLENCIA,
} Biome;

typedef struct {
    char name[100];
    int totalCoins;
    int totalFragments;
    float totalTime;
} Score;

typedef enum { RIGHT, LEFT, UP, DOWN } Direction;

typedef enum { MENU_MAIN_CONTENT, MENU_CUTSCENE1, MENU_CUTSCENE2, MENU_CUTSCENE3, MENU_CUTSCENE4, MENU_CUTSCENE5, GAME_MAIN_CONTENT, GAME_DEATH, CREDITS_CUTSCENE1, CREDITS_CUTSCENE2, CREDITS_CUTSCENE3, CREDITS_ADD_SCORE, CREDITS_SCORE, CREDITS_FINAL } GameState;

extern GameState state;

extern Font InfernoFont;

static inline int max(int a, int b) { return (a > b) ? a : b; }

static inline int min(int a, int b) { return (a < b) ? a : b; }

static inline void drawCenteredText(const char* text, int x, int y, int fontSize, Color color) {
    int hw = MeasureText(text, fontSize) >> 1;
    DrawText(text, x - hw, y, fontSize, color);
}

static inline Color getNegativeColor(Color color) {
    return (Color) { (color.r + 128) & 255, (color.g + 128) & 255, (color.b + 128) & 255, 255 };
}

static inline Color LerpColor(Color a, Color b, float t) {
    Color result;
    result.r = a.r + (b.r - a.r) * t;
    result.g = a.g + (b.g - a.g) * t;
    result.b = a.b + (b.b - a.b) * t;
    result.a = a.a + (b.a - a.a) * t;
    return result;
}

#endif
