#ifndef AUDIO_H
#define AUDIO_H

#include <raylib.h>
#include "common.h"

typedef struct Audio {
  Music* musics;
  int musicsLength;

  Sound* sounds;
  int soundsLength;

  void (*loadSound)(struct Audio*, const char*, int);
  void (*loadMusic)(struct Audio*, const char*, int);
  void (*loadMusics)(struct Audio*, const char**);
  void (*loadSounds)(struct Audio*, const char**);

  void (*resumeMusic)(struct Audio*, int);
  void (*pauseMusic)(struct Audio*, int);

  void (*updateMusic)(struct Audio*, int);

  void (*playSound)(struct Audio*, int);
  void (*free)(struct Audio*);
} Audio;

Audio* new_Audio(const int musicsLength, const int soundsLength);

#endif