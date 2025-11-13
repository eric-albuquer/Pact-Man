#include "audio.h"
#include <stdlib.h>

static void pauseMusic(Audio* this, int idx) {
    PauseMusicStream(this->musics[idx]);
}

static void resumeMusic(Audio* this, int idx) {
    ResumeMusicStream(this->musics[idx]);
}

static void updateMusic(Audio* this, int idx) {
    UpdateMusicStream(this->musics[idx]);
}

static void loadSound(Audio* this, const char* path, int idx){
    this->sounds[idx] = LoadSound(path);
}

static void loadMusic(Audio* this, const char* path, int idx){
    this->musics[idx] = LoadMusicStream(path);
    PlayMusicStream(this->musics[idx]);
}

static void playSound(Audio* this, int idx){
    PlayAudioStream(this->sounds[idx].stream);
}

static void _free(Audio* this) {
    for (int i = 0; i < this->musicsLength; i++){
        UnloadMusicStream(this->musics[i]);
    }

    free(this->musics);

    for (int i = 0; i < this->soundsLength; i++){
        UnloadSound(this->sounds[i]);
    }

    free(this->sounds);
    
    free(this);
}

Audio* new_Audio(const int musicsLength, const int soundsLength) {
    Audio* this = malloc(sizeof(Audio));

    this->musicsLength = musicsLength;
    this->musics = malloc(sizeof(Music) * musicsLength);

    this->soundsLength = soundsLength;
    this->sounds = malloc(sizeof(Sound) * soundsLength);

    this->resumeMusic = resumeMusic;
    this->pauseMusic = pauseMusic;
    this->updateMusic = updateMusic;
    this->playSound = playSound;
    this->loadMusic = loadMusic;
    this->loadSound = loadSound;
    this->free = _free;

    return this;
}