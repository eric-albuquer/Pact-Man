#ifndef CHEATS_H
#define CHEATS_H

#include "controller.h"
#include "common.h"

void loadCheats();
void unloadCheats();

int hasCheat(InputBuffer* inputs);

#endif