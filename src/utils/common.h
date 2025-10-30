#ifndef COMMON_H
#define COMMON_H

typedef enum { RIGHT, LEFT, UP, DOWN } Direction;

typedef enum { MENU, GAME } GameState;

typedef struct {
    int x;
    int y;
} VecInt2;

extern GameState state;

static inline int max(int a, int b) { return (a > b) ? a : b; }

static inline int min(int a, int b) { return (a < b) ? a : b; }

#endif
