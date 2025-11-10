#ifndef SOUNDS_H
#define SOUNDS_H

#include <raylib.h>
#include "common.h"

typedef struct Sounds {
  Music gulaMusic;
  Music heresiaMusic;
  Music violenciaMusic;
  Music luxuriaMusic;

  void (*resumeMusic)(struct Sounds*, Biome);
  void (*pauseMusic)(struct Sounds*, Biome);

  void (*updateMusic)(struct Sounds*, Biome);
  void (*free)(struct Sounds*);
} Sounds;

Sounds* new_Sounds(const char* gula, const char* heresia, const char* violencia, const char* luxuria);

#endif