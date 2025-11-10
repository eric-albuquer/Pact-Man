#include "sounds.h"
#include <stdlib.h>

static void applyFunction(Sounds* this, Biome biome, void (*func)(Music)){
    if (biome == GULA) func(this->gulaMusic);
    else if (biome == HERESIA) func(this->heresiaMusic);
    else if (biome == VIOLENCIA) func(this->violenciaMusic);
    else if (biome == LUXURIA) func(this->luxuriaMusic);
}

static void pauseMusic(Sounds* this, Biome biome) {
    applyFunction(this, biome, PauseMusicStream);
}

static void resumeMusic(Sounds* this, Biome biome) {
    applyFunction(this, biome, ResumeMusicStream);
}

static void updateMusic(Sounds* this, Biome biome) {
    applyFunction(this, biome, UpdateMusicStream);
}

static void _free(Sounds* this) {
    UnloadMusicStream(this->gulaMusic);
    UnloadMusicStream(this->gulaMusic);
    UnloadMusicStream(this->gulaMusic);
    UnloadMusicStream(this->gulaMusic);
    free(this);
}

Sounds* new_Sounds(const char* gula, const char* heresia, const char* violencia, const char* luxuria) {
    Sounds* this = malloc(sizeof(Sounds));

    this->gulaMusic = LoadMusicStream(gula);
    this->heresiaMusic = LoadMusicStream(heresia);
    this->violenciaMusic = LoadMusicStream(violencia);
    this->luxuriaMusic = LoadMusicStream(luxuria);

    PlayMusicStream(this->gulaMusic);
    PlayMusicStream(this->heresiaMusic);
    PlayMusicStream(this->violenciaMusic);
    PlayMusicStream(this->luxuriaMusic);

    this->resumeMusic = resumeMusic;
    this->pauseMusic = pauseMusic;
    this->updateMusic = updateMusic;
    this->free = _free;

    return this;
}