#ifndef CHEATS_H
#define CHEATS_H

#include "common.h"

typedef enum {
    CHEAT_UNKNOWN,
    CHEAT_INFINITY_INVULNERABILITY,
    CHEAT_INFINITY_REGENERATION,
    CHEAT_INFINITY_INVISIBILITY,
    CHEAT_INFINITY_FREEZE_TIME,
    CHEAT_INFINITY_SPEED,
    CHEAT_ADD_FRAGMENT,

    CHEAT_COUNT,
} Cheats;

void loadCheats();
void unloadCheats();

void resetCheatPointer();
int hasCheat(int code);

#endif