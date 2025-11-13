#include "audio.h"
#include <stdlib.h>

static void pauseMusic(Audio* this, int idx) {
    PauseMusicStream(this->musics[idx]);
}

static void resumeMusic(Audio* this, int idx) {
    ResumeMusicStream(this->musics[idx]);
}

static void updateMusic(Audio* this, int idx) {
    SetMusicVolume(this->musics[idx], this->musicVolume);
    UpdateMusicStream(this->musics[idx]);
}

static void loadSound(Audio* this, const char* path, int idx){
    this->sounds[idx] = LoadSound(path);
}

static void loadMusic(Audio* this, const char* path, int idx){
    this->musics[idx] = LoadMusicStream(path);
    PlayMusicStream(this->musics[idx]);
}

static void setSoundVolume(Audio* this, float soundVolume){
    if (soundVolume > 1){
        this->soundVolume = 1.0;
    } else if (soundVolume < 0){
        this->soundVolume = 0;
    } else {
        this->soundVolume = soundVolume;
    }
}

static void setMusicVolume(Audio* this, float musicVolume){
    if (musicVolume > 1){
        this->musicVolume = 1.0;
    } else if (musicVolume < 0){
        this->musicVolume = 0;
    } else {
        this->musicVolume = musicVolume;
    }
}

static void playSound(Audio* this, int idx){
    AudioStream stream = this->sounds[idx].stream;
    SetAudioStreamVolume(stream, this->soundVolume);
    PlayAudioStream(stream);
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

    this->soundVolume = 1.0f;
    this->musicVolume = 1.0f;

    this->setMusicVolume = setMusicVolume;
    this->setSoundVolume = setSoundVolume;
    this->resumeMusic = resumeMusic;
    this->pauseMusic = pauseMusic;
    this->updateMusic = updateMusic;
    this->playSound = playSound;
    this->loadMusic = loadMusic;
    this->loadSound = loadSound;
    this->free = _free;

    return this;
}