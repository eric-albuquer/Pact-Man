#ifndef COMMON_H
#define COMMON_H

#include <raylib.h>

//===============================================================
//  🌐 Variáveis Globais
//===============================================================
#define HUD_OPACITY (Color){ 0, 0, 0, 200 }       // Opacidade do HUD

// ######################################################################################################################################################################### MAPA
//===============================================================
//  ⚔️ Dano e Combate
//===============================================================
#define FIRE_DAMAGE 10                             // Dano de fogo
#define SPIKE_DAMAGE 4                             // Dano de espinhos
#define ENEMY_DAMAGE 15                            // Dano de inimigos
#define DEGENERATION_DAMAGE 1                      // Dano por degeneração

//===============================================================
//  💰 Economia e Recompensas
//===============================================================
#define COINS_TO_FRAGMENT 100                      // Moedas necessárias para fragmentos
#define PACMAN_KILL_COINS 10                       // Moedas por matar Pacman
#define PACMAN_KILL_HEALTH 20                      // Vida recuperada ao matar Pacman

//===============================================================
//  ⏳ Duração de Efeitos (Buffs/Debuffs)
//===============================================================
#define MUD_SLOWNESS_DURATION 3                    // Duração lentidão por lama
#define SPIKE_SLOWNESS_DURATION 2                  // Duração lentidão por espinhos
#define TENTACLE_SLOWNESS_DURATION 4               // Duração lentidão por tentáculos

#define FRUIT_INVULNERABILITY_DURATION \
    ((const int[DIFICULTY_COUNT]){ 120, 90, 70 }[dificulty]) // Invulnerabilidade de frutas

#define FREEZE_TIME_DURATION 90                     // Duração do congelamento do tempo
#define DEGENERATION_DURATION 5                     // Duração da degeneração
#define INVISIBILITY_DURATION 80                    // Duração da invisibilidade
#define FONT_REGENERATION_DURATION 2                // Duração da regeneração de fonte
#define POTION_REGENERATION_DURATION 50            // Duração da regeneração de poção
#define BONUS_DELAY 24                              // Delay entre bônus

//===============================================================
//  ✨ Regeneração e Saúde
//===============================================================
#define FONT_REGENERATION_STRENGTH 5               // Força da regeneração de fonte
#define POTION_REGENERATION_STRENGTH 2            // Força da regeneração de poção

//===============================================================
//  🤖 Inteligência Artificial (IA)
//===============================================================
#define MAX_PERSUIT_RADIUS_BIOME(i) \
    ((const int[DIFICULTY_COUNT][4]){ \
        {30, 35, 40, 45}, \
        {50, 55, 60, 65}, \
        {70, 75, 80, 85} \
    }[dificulty][i])                            // Raio máximo de perseguição por bioma

#define BEST_PATH_PROBABILITY_BIOME(i) \
    ((const int[DIFICULTY_COUNT][4]){ \
        {30, 35, 40, 45}, \
        {40, 50, 60, 70}, \
        {60, 70, 80, 85}  \
    }[dificulty][i])                            // Probabilidade de seguir melhor caminho por bioma

//===============================================================
//  💥 Chefes (Bosses)
//===============================================================
#define BOSS_FIRE_QUANTITY 60                      // Quantidade de fogo do boss
#define BOSS_TENTACLE_QUANTITY 15                  // Quantidade de tentáculos do boss
#define FINAL_BOSS_GRAVE_PROBABILITY 1            // Probabilidade de túmulo no boss final
#define FINAL_BOSS_SPIKE_PROBABILITY 20           // Probabilidade de espinhos no boss final

//===============================================================
//  🗺️ Mapa e Ambiente
//===============================================================
#define BIOME_DEGEN_START_TIME \
    ((const float[DIFICULTY_COUNT]){ 240.0f, 180.0f, 120.0f }[dificulty]) // Tempo de degeneração por bioma

#define BATERY_DECAY \
    ((const float[DIFICULTY_COUNT]){ 0.001f, 0.002f, 0.003f }[dificulty]) // Decaimento de bateria

#define SPEED_DECAY 2                               // Decaimento de velocidade
#define SPEED_RELOAD 2                              // Recarga de velocidade

#define HEAVEN_SPEED 5

//===============================================================
//  ⏸️ DELAY DE INPUTS
//===============================================================
#define PAUSE_DELAY 1.0f                            // Delay do pause
#define MAX_CHEAT_UPDATES_DELAY 20                  // Máx. delay para cheats

// ######################################################################################################################################################################### CHUNK LOADER
//===============================================================
//  👾 Geração de Inimigos por Bioma
//===============================================================
#define BIOME_ENEMY_PROBABILITY (int[4]){ 50, 40, 30, 20 }  // Probabilidade de inimigos por bioma

//===============================================================
//  🧱 Densidades de Obstáculos e Elementos
//===============================================================
#define WALL_DENSITY 75                             // Densidade de paredes
#define MUD_DENSITY 5                               // Densidade de lama
#define GRAVE_DENSITY 5                             // Densidade de túmulos
#define GRAVE_INFESTED_PROBABILITY 5               // Probabilidade de túmulos infestados
#define FIRE_DENSITY 30                             // Densidade de fogo
#define SPIKE_DENSITY 5                             // Densidade de espinhos
#define DEGENERATION_PROBABILITY 50

//===============================================================
//  🪙 Densidades de Itens Colecionáveis
//===============================================================
#define COIN_DENSITY 15                             // Densidade de moedas
#define FRUIT_PROBABILITY 8                         // Probabilidade de frutas

//===============================================================
//  ✨ Probabilidades de Power-Ups
//===============================================================
#define INVISIBILITY_PROBABILITY 8                  // Probabilidade de invisibilidade
#define REGENERATION_PROBABILITY 8                  // Probabilidade de regeneração
#define BATERY_PROBABILITY 50                        // Probabilidade de baterias
#define FREEZE_TIME_PROBABILITY 4                   // Probabilidade de congelamento do tempo

//===============================================================
//  ENUNS GLOBAIS
//===============================================================

typedef enum {
    LUXURIA,
    GULA,
    HERESIA,
    VIOLENCIA,
} Biome;

typedef enum {
    DIFICULTY_EASY,
    DIFICULTY_MEDIUM,
    DIFICULTY_HARD,

    DIFICULTY_COUNT,
} Dificulty;

typedef enum {
    RIGHT,
    LEFT,
    UP,
    DOWN
} Direction;

typedef enum { MENU_MAIN_CONTENT, MENU_CUTSCENE1, MENU_CUTSCENE2, MENU_CUTSCENE3, MENU_CUTSCENE4, MENU_CUTSCENE5, GAME_MAIN_CONTENT, GAME_DEATH, CREDITS_CUTSCENE1, CREDITS_CUTSCENE2, CREDITS_CUTSCENE3, CREDITS_ADD_SCORE, CREDITS_SCORE, CREDITS_FINAL } GameState;

//===============================================================
//  STRUCTS GLOBAIS
//===============================================================

typedef struct {
    char name[100];
    int totalCoins;
    int totalFragments;
    float totalTime;
    int dificulty;
} Score;

//===============================================================
//  VARIÁVEIS GLOBAIS
//===============================================================

extern GameState state;

extern Font InfernoFont;

extern Dificulty dificulty;

//===============================================================
//  FUNÇÕES GLOBAIS AUXILIARES
//===============================================================

static inline int max(int a, int b) { return (a > b) ? a : b; }

static inline int min(int a, int b) { return (a < b) ? a : b; }

static inline void drawCenteredText(const char* text, int x, int y, int fontSize, Color color) {
    int hw = MeasureText(text, fontSize) >> 1;
    DrawText(text, x - hw, y, fontSize, color);
}

static inline Color getNegativeColor(Color color) {
    return (Color) { (color.r + 128) & 255, (color.g + 128) & 255, (color.b + 128) & 255, 255 };
}

static inline Color LerpColor(Color a, Color b, float t) {
    Color result;
    result.r = a.r + (b.r - a.r) * t;
    result.g = a.g + (b.g - a.g) * t;
    result.b = a.b + (b.b - a.b) * t;
    result.a = a.a + (b.a - a.a) * t;
    return result;
}

#endif
