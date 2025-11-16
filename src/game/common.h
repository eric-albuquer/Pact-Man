#ifndef COMMON_H
#define COMMON_H

#include <raylib.h>

typedef enum {
    LUXURIA,
    GULA,
    HERESIA,
    VIOLENCIA,
} Biome;

typedef enum { RIGHT, LEFT, UP, DOWN } Direction;

typedef enum { MENU_MAIN_CONTENT, MENU_CUTSCENE1, MENU_CUTSCENE2, MENU_CUTSCENE3, MENU_CUTSCENE4, MENU_CUTSCENE5, GAME_MAIN_CONTENT, GAME_DEATH, CREDITS_CUTSCENE1, CREDITS_CUTSCENE2, CREDITS_SCORE, CREDITS_FINAL } GameState;

extern GameState state;

extern Font InfernoFont;

static inline int max(int a, int b) { return (a > b) ? a : b; }

static inline int min(int a, int b) { return (a < b) ? a : b; }

#endif
