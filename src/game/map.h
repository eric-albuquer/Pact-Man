#ifndef MAP_H
#define MAP_H

#include "arraylist.h"
#include "controler.h"
#include "hashtable.h"
#include "linkedlist.h"
#include "player.h"
#include "chunk_manager.h"

//===============================================================
//  ⚔️ Constantes de Dano e Combate
//===============================================================

#define FIRE_DAMAGE 10
#define SPIKE_DAMAGE 2 
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
#define FRUIT_INVULNERABILITY_DURATION 100
#define FREEZE_TIME_DURATION 90
#define DEGENERATION_DURATION 5
#define INVISIBILITY_DURATION 100
#define FONT_REGENERATION_DURATION 2
#define POTION_REGENERATION_DURATION 50
#define BONUS_DELAY 48

//===============================================================
//  ✨ Constantes de Regeneração e Saúde
//===============================================================

#define FONT_REGENERATION_STRENGTH 5
#define POTION_REGENERATION_STRENGTH 2

//===============================================================
//  🤖 Constantes de Inteligência Artificial (IA)
//===============================================================

#define MAX_PERSUIT_RADIUS_BIOME (const int[4]) {40, 45, 50, 55}
#define BEST_PATH_PROBABILITY_BIOME (const int[4]) {40, 50, 60, 70}

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

#define BIOME_DEGEN_START_TIME 180.0f
#define BATERY_DECAY 0.003f

typedef struct Map {
    ChunkManager* manager;
    ArrayList* changedChunk;

    LinkedList* firedCells;
    LinkedList* tentacleCells;

    float biomeTime;

    Player* player;
    Score lastScore;
    unsigned int updateCount;

    void (*update)(struct Map*, Controler*, float);
    void (*restart)(struct Map*);
    void (*free)(struct Map*);
} Map;

Map* new_Map(int chunkCols, int chunkRows, int spawnX, int spawnY);

#endif
