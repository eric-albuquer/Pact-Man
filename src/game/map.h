#ifndef MAP_H
#define MAP_H

#include "arraylist.h"
#include "controller.h"
#include "hashtable.h"
#include "linkedlist.h"
#include "player.h"
#include "chunk_manager.h"
#include "common.h"

//===============================================================
//  ⚔️ Constantes de Dano e Combate
//===============================================================

#define FIRE_DAMAGE 10
#define SPIKE_DAMAGE 4 
#define ENEMY_DAMAGE 15
#define DEGENERATION_DAMAGE 1

//===============================================================
//  💰 Constantes de Economia e Recompensa
//===============================================================

#define COINS_TO_FRAGMENT 100
#define PACMAN_KILL_COINS 10
#define PACMAN_KILL_HEALTH 20

//===============================================================
//  ⏳ Constantes de Duração de Efeitos (Buffs/Debuffs)
//===============================================================

#define MUD_SLOWNESS_DURATION 3
#define SPIKE_SLOWNESS_DURATION 2 
#define TENTACLE_SLOWNESS_DURATION 4 

#define FRUIT_INVULNERABILITY_DURATION \
((const int[DIFICULTY_COUNT]){ \
        120, \
        90, \
        70 \
}[dificulty]) 

#define FREEZE_TIME_DURATION 90
#define DEGENERATION_DURATION 5
#define INVISIBILITY_DURATION 80
#define FONT_REGENERATION_DURATION 2
#define POTION_REGENERATION_DURATION 50
#define BONUS_DELAY 24

//===============================================================
//  ✨ Constantes de Regeneração e Saúde
//===============================================================

#define FONT_REGENERATION_STRENGTH 5
#define POTION_REGENERATION_STRENGTH 2

//===============================================================
//  🤖 Constantes de Inteligência Artificial (IA)
//===============================================================

#define MAX_PERSUIT_RADIUS_BIOME(i) \
    ((const int[DIFICULTY_COUNT][4]){ \
        {30, 35, 40, 45}, \
        {50, 55, 60, 65}, \
        {70, 75, 80, 85} \
    }[dificulty][i])

#define BEST_PATH_PROBABILITY_BIOME(i) \
    ((const int[DIFICULTY_COUNT][4]){ \
        {30, 35, 40, 45}, \
        {40, 50, 60, 70}, \
        {60, 70, 80, 85}  \
    }[dificulty][i])

//===============================================================
//  💥 Constantes de Chefes (Bosses)
//===============================================================

#define BOSS_FIRE_QUANTITY 60
#define BOSS_TENTACLE_QUANTITY 15
#define FINAL_BOSS_GRAVE_PROBABILITY 1
#define FINAL_BOSS_SPIKE_PROBABILITY 20

//===============================================================
//  🗺️ Constantes de Mapa e Ambiente
//===============================================================

#define BIOME_DEGEN_START_TIME \
    ((const float[DIFICULTY_COUNT]){ \
        240.0f, \
        180.0f, \
        120.0f \
    }[dificulty])

#define BATERY_DECAY 0.003f

#define SPEED_DECAY 2
#define SPEED_RELOAD 2

#define PAUSE_DELAY 1.0f
#define MAX_CHEAT_UPDATES_DELAY 20
#define MIN_CHEAT_UPDATES_DELAY 2

typedef struct Map {
    ChunkManager* manager;
    ArrayList* changedChunk;

    LinkedList* firedCells;
    LinkedList* tentacleCells;

    float biomeTime;

    Player* player;
    Score lastScore;
    unsigned int updateCount;

    bool running;

    void (*update)(struct Map*, Controller*, float);
    void (*restart)(struct Map*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(int chunkCols, int chunkRows, int spawnX, int spawnY);

#endif
