#include "map.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "chunk.h"
#include "enemy.h"
#include "pathfinding.h"
#include "chunk_loader.h"

//===============================================================
//  FUNÇÕES AUXILIARES DA ATUALIZAÇÃO DO PLAYER
//===============================================================

static inline void updatePlayerBiome(Map* this, Cell* cell) {
    Player* p = this->player;
    if (cell->biome <= p->biome) return;
    p->biome = cell->biome;
    p->biomeCoins = 0;
    p->biomeFragment = 0;
    p->fragmentByCoins = false;
    p->batery = 1.0f;

    this->biomeTime = 0.0f;
}

static inline void collectItens(Player* p, Cell* cell, bool isFreeze) {
    if (cell->type == CELL_COIN) {
        cell->type = CELL_EMPTY;
        p->biomeCoins++;
        p->totalCoins++;
    } else if (cell->type == CELL_FRAGMENT) {
        cell->type = CELL_EMPTY;
        p->totalFragment++;
        p->biomeFragment++;
    } else if (cell->type == CELL_BATERY) {
        cell->type = CELL_EMPTY;
        p->batery = 1.0f;
    } else if (cell->type == CELL_FREEZE_TIME) {
        cell->type = CELL_EMPTY;
    } else if (cell->type == CELL_INVISIBILITY) {
        cell->type = CELL_EMPTY;
    } else if (cell->type == CELL_REGENERATION) {
        cell->type = CELL_EMPTY;
    } else if (cell->type == CELL_FRUIT && !isFreeze) {
        cell->type = CELL_EMPTY;
    }
    if (p->fragmentByCoins == false && p->biomeCoins / COINS_TO_FRAGMENT) {
        p->fragmentByCoins = true;
        p->biomeFragment++;
        p->totalFragment++;
    }
}

static inline void applyRandomGoodEffects(Player* p) {
    int idx = rand() % 4;
    if (idx == 0) p->effects.invulnerability.duration = FRUIT_INVULNERABILITY_DURATION;
    else if (idx == 1) {
        p->effects.regeneration.duration = POTION_REGENERATION_DURATION;
        p->effects.regeneration.strenght = max(POTION_REGENERATION_STRENGTH, p->effects.regeneration.strenght);
    } else if (idx == 2) p->effects.invisibility.duration = INVISIBILITY_DURATION;
    else if (idx == 3) p->effects.freezeTime.duration = FREEZE_TIME_DURATION;
}

static inline void applyPlayerEffects(Player* p, Cell* cell, unsigned int updateCount) {
    static const int mudDuration = (MUD_SLOWNESS_DURATION << 1) - 1;
    static const int spikeDuration = (SPIKE_SLOWNESS_DURATION << 1) - 1;
    static const int mudDurationLimit = (MUD_SLOWNESS_DURATION << 1) - 2;
    static const int spikeDurationLimit = (SPIKE_SLOWNESS_DURATION << 1) - 2;

    if (cell->type == CELL_MUD && p->effects.slowness.duration < mudDurationLimit) {
        p->effects.slowness.duration = mudDuration;
    } else if (cell->type == CELL_SPIKE && p->effects.slowness.duration < spikeDurationLimit) {
        p->effects.slowness.duration = spikeDuration;
    } else if (cell->type == CELL_FRUIT && p->effects.freezeTime.duration == 0) {
        p->effects.invulnerability.duration = FRUIT_INVULNERABILITY_DURATION;
    } else if (cell->type == CELL_FONT_HEALTH) {
        p->effects.regeneration.duration = FONT_REGENERATION_DURATION;
        p->effects.regeneration.strenght = max(FONT_REGENERATION_STRENGTH, p->effects.regeneration.strenght);
    } else if (cell->type == CELL_BONUS && updateCount % BONUS_DELAY == 0) {
        applyRandomGoodEffects(p);
    } else if (isDegenerated(cell->type)) {
        p->effects.degeneration.duration = DEGENERATION_DURATION;
        p->effects.degeneration.strenght = (cell->type - CELL_DEGENERATED_1) + 1;
    } else if (cell->type == CELL_INVISIBILITY) {
        p->effects.invisibility.duration = INVISIBILITY_DURATION;
    } else if (cell->type == CELL_REGENERATION) {
        p->effects.regeneration.duration = POTION_REGENERATION_DURATION;
        p->effects.regeneration.strenght = max(POTION_REGENERATION_STRENGTH, p->effects.regeneration.strenght);
    } else if (cell->type == CELL_TENTACLE && p->effects.slowness.duration < spikeDurationLimit) {
        p->effects.slowness.duration = spikeDuration;
    } else if (cell->type == CELL_FREEZE_TIME) {
        p->effects.freezeTime.duration = FREEZE_TIME_DURATION;
    }
}

static inline void updatePlayerEffects(Player* p, Cell* cell) {
    if (p->effects.slowness.duration > 0) {
        p->effects.slowness.duration--;
    }

    if (p->effects.freezeTime.duration > 0) {
        p->effects.freezeTime.duration--;
    }

    if (p->effects.invulnerability.duration > 0) {
        p->effects.invulnerability.duration--;
    }

    if (p->effects.invisibility.duration > 0) {
        p->effects.invisibility.duration--;
    }

    if (p->effects.regeneration.duration > 0) {
        p->life = min(p->effects.regeneration.strenght + p->life, START_LIFE);
        p->effects.regeneration.duration--;
    } else
        p->effects.regeneration.strenght = 0;

    if (p->effects.degeneration.duration > 0) {
        p->life -= DEGENERATION_DAMAGE;
        p->damaged = true;
        p->effects.degeneration.duration--;
    } else
        p->effects.degeneration.strenght = 0;

    if (p->batery >= BATERY_DECAY)
        p->batery -= BATERY_DECAY;
}

static inline void updateDamagePlayer(Player* p, Cell* cell) {
    if (p->effects.invulnerability.duration > 0) return;
    if (cell->type == CELL_FIRE_ON) {
        p->life -= FIRE_DAMAGE;
        p->damaged = true;
    } else if (cell->type == CELL_SPIKE) {
        p->life -= SPIKE_DAMAGE;
        p->damaged = true;
    }

    if (p->life < 0) {
        p->life = 0;
        p->alive = false;
        state = GAME_DEATH;
    }
}

static inline void updatePlayerEndGame(Map* this, Cell* cell) {
    this->player->effects = (Effects){ 0 };
    if (cell->type != CELL_PORTAL)
        cell->type = CELL_HEAVEN;
    else {
        state = CREDITS_CUTSCENE1;
        this->restart(this);
    }
}

//===============================================================
//  FUNÇÕES DE MOVIMENTAÇÃO DO PLAYER
//===============================================================

static const Vec2i DIR_VECTOR[4] = {
    {1, 0},
    {-1, 0},
    {0, -1},
    {0, 1}
};

static inline Cell* movePlayer(Map* this, Vec2i newDir) {
    Player* p = this->player;
    ChunkManager* cm = this->manager;
    int playerNextX = p->x + newDir.x;
    int playerNextY = p->y + newDir.y;

    Cell* nextCell = cm->getUpdatedCell(cm, playerNextX, playerNextY);
    if (nextCell == NULL) return NULL;

    bool cantPassBiome = p->biomeFragment < 2 && nextCell->biome > p->biome;
    if (cantPassBiome || nextCell->biome < p->biome) return NULL;
    if (p->effects.invisibility.duration == 0 && !isPassable(nextCell->type) && isPassable(p->cellType)) return NULL;

    p->x = playerNextX;
    p->y = playerNextY;

    updatePlayerBiome(this, nextCell);

    p->updateDirection(p);

    if (p->updateChunk(p)) cm->loadAdjacents(cm);
    return nextCell;
}

static inline void updatePlayerWind(Map* this, Cell* cell) {
    if (isWind(cell->type)) {
        Vec2i dir = DIR_VECTOR[cell->type - CELL_WIND_RIGHT];
        movePlayer(this, dir);
    }
}

static inline void updatePlayerByInput(Map* this, Cell* cell, Input input) {
    Player* p = this->player;
    static Vec2i dir[4];
    int length = 0;

    if (p->dir == RIGHT) {
        if (input.left) dir[length++] = (Vec2i){ -1, 0 };
        if (input.up) dir[length++] = (Vec2i){ 0, -1 };
        if (input.down) dir[length++] = (Vec2i){ 0, 1 };
        if (input.right) dir[length++] = (Vec2i){ 1, 0 };
    } else if (p->dir == LEFT) {
        if (input.right) dir[length++] = (Vec2i){ 1, 0 };
        if (input.up) dir[length++] = (Vec2i){ 0, -1 };
        if (input.down) dir[length++] = (Vec2i){ 0, 1 };
        if (input.left) dir[length++] = (Vec2i){ -1, 0 };
    } else if (p->dir == UP) {
        if (input.down) dir[length++] = (Vec2i){ 0, 1 };
        if (input.right) dir[length++] = (Vec2i){ 1, 0 };
        if (input.left) dir[length++] = (Vec2i){ -1, 0 };
        if (input.up) dir[length++] = (Vec2i){ 0, -1 };
    } else if (p->dir == DOWN) {
        if (input.up) dir[length++] = (Vec2i){ 0, -1 };
        if (input.right) dir[length++] = (Vec2i){ 1, 0 };
        if (input.left) dir[length++] = (Vec2i){ -1, 0 };
        if (input.down) dir[length++] = (Vec2i){ 0, 1 };
    }

    for (int i = 0; i < length; i++) {
        Cell* nextCell = movePlayer(this, dir[i]);
        if (nextCell) {
            cell = nextCell;
            break;
        }
    }
}

static inline void updatePlayerMovement(Map* this, Cell* cell, Input input) {
    Player* p = this->player;
    p->lastX = p->x;
    p->lastY = p->y;

    if ((p->effects.slowness.duration & 1) == 0)
        updatePlayerByInput(this, cell, input);

    updatePlayerWind(this, cell);
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DO PLAYER
//===============================================================

static inline void updatePlayer(Map* this, Input input) {
    Player* p = this->player;
    ChunkManager* cm = this->manager;
    Cell* cell = cm->getUpdatedCell(cm, p->x, p->y);

    p->hitEnemy = false;
    p->damaged = false;

    if (cm->heaven)
        updatePlayerEndGame(this, cell);
    applyPlayerEffects(p, cell, this->updateCount);

    p->cellType = cell->type;

    updatePlayerMovement(this, cell, input);
    bool isFreeze = p->effects.freezeTime.duration > 0;
    updatePlayerEffects(p, cell);

    updateDamagePlayer(p, cell);
    collectItens(p, cell, isFreeze);
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DOS INIMIGOS
//===============================================================

static inline void updateEnemyChunk(ChunkManager* cm, Node* node, LinkedList* list, ArrayList* changedChunk) {
    Enemy* e = node->data;
    if (!e->updateChunk(e)) return;
    e->changedChunk = true;
    list->removeNode(list, node);
    Chunk* newChunk = cm->getLoadedChunk(cm, e->chunkX, e->chunkY);
    newChunk->enemies->addLast(newChunk->enemies, e);
    changedChunk->push(changedChunk, e);
}

static inline bool isFarAwayFromSpawn(Player* p, Enemy* e) {
    float distToSpawn = hypotf(p->x - e->spawnX, p->y - e->spawnY);

    return distToSpawn > MAX_PERSUIT_RADIUS_BIOME[e->biome];
}

static inline void findNewCell(ChunkManager* cm, Enemy* e) {
    int idx = rand() & 7;
    Chunk** adjacents = cm->adjacents;
    Chunk* chunk;

    for (int i = 0; i < 8; i++) {
        chunk = adjacents[ADJACENT_IDX[idx]];
        if (chunk != NULL && chunk->biome == e->biome) break;
        idx = (idx + 1) & 7;
    }

    int cIdx = rand() & TOTAL_CELLS_MASK;

    for (int i = 0; i < 256; i++) {
        if (isPassable(chunk->cells[cIdx].type) && chunk->cells[cIdx].biome == e->biome) break;
        cIdx = (cIdx + 1) & TOTAL_CELLS_MASK;
    }

    e->needToTeleport = false;
    e->x = (cIdx & CHUNK_MASK) + (chunk->x << CHUNK_SHIFT);
    e->y = (cIdx >> CHUNK_SHIFT) + (chunk->y << CHUNK_SHIFT);
}

//===============================================================
//  FUNÇÕES DE MOVIMENTAÇÃO DO INIMIGO
//===============================================================

static inline void updateEnemyMovement(ChunkManager* cm, Enemy* e, Player* p) {
    e->lastX = e->x;
    e->lastY = e->y;

    if (e->needToTeleport) {
        findNewCell(cm, e);
        return;
    }

    if (p->effects.freezeTime.duration > 0) return;

    NextPos pos = getNextPos(cm, e->x, e->y, e->biome);
    if (pos.moves == 0) return;

    sortNextPos(&pos);

    bool normalMovement = rand() % 100 < BEST_PATH_PROBABILITY_BIOME[e->biome];

    Vec2i nextPos = normalMovement ? getBestPos(pos) : getRandomPos(pos);
    if (isFarAwayFromSpawn(p, e))
        nextPos = getCloserToSpawn(pos, e->spawnX, e->spawnY);
    else if (p->effects.invisibility.duration > 0 && normalMovement)
        nextPos = getRandomPos(pos);
    else if (p->effects.invulnerability.duration > 0 && normalMovement)
        nextPos = getWorstPos(pos);

    e->x = nextPos.x;
    e->y = nextPos.y;

    e->updateDirection(e);
}

//===============================================================
//  FUNÇÕES DE ATUALIZAÇÃO DO BOSS
//===============================================================

static inline void bossDestroyMap(ChunkManager* cm, Enemy* e, LinkedList* firedCells) {
    if (e->biome == VIOLENCIA){
        for (int i = -5; i < 6; i++){
            for (int j = -5; j < 6; j++){
                Cell* cell = cm->getUpdatedCell(cm, e->x + j, e->y + i);
                if (cell == NULL) continue;
                if (cell->type == CELL_EMPTY){
                    int num = rand() % 1000;
                    if (num < FINAL_BOSS_GRAVE_PROBABILITY)
                        cell->type = CELL_GRAVE_INFESTED;
                    else if (num < FINAL_BOSS_SPIKE_PROBABILITY)
                        cell->type = CELL_SPIKE;
                }
            }
        }
    }

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            Cell* cell = cm->getUpdatedCell(cm, e->x + j, e->y + i);
            if (cell && cell->type != CELL_FRAGMENT) {
                if (e->biome >= 1) {
                    if (cell->type != CELL_FIRE_ON)
                        firedCells->addLast(firedCells, cell);
                    cell->type = CELL_FIRE_ON;
                } else {
                    cell->type = CELL_EMPTY;
                }
            }
        }
    }
}

static inline void bossTentacle(ChunkManager* cm, Enemy* e, LinkedList* tentacleCells) {
    int x = e->x + (rand() & 31) - 16;
    int y = e->y + (rand() & 31) - 16;
    Cell* cell = cm->getLoadedCell(cm, x, y);
    if (cell) {
        tentacleCells->addLast(tentacleCells, cell);
        cell->type = CELL_TENTACLE;
    }
}

static inline void bossMecanics(ChunkManager* cm, Enemy* e, LinkedList* firedCells, LinkedList* tentacleCells) {
    bossDestroyMap(cm, e, firedCells);
    bossTentacle(cm, e, tentacleCells);
}

//===============================================================
//  FUNÇÕES DE COLISÃO DO INIMIGO COM O PLAYER
//===============================================================

static inline bool handlePlayerEnemyColision(ChunkManager* cm, Node* node, LinkedList* enemies, Player* p) {
    Enemy* e = node->data;

    int start = e->isBoss;
    for (int i = -start; i <= start; i++) {
        for (int j = -start; j <= start; j++) {
            if ((e->chunkX == p->chunkX && e->chunkY == p->chunkY) && ((e->lastX + j == p->lastX && e->lastY + i == p->lastY) || (e->x + j == p->lastX && e->y + i == p->lastY))) {
                if (p->effects.invulnerability.duration > 0) {
                    if (e->isBoss) {
                        p->biomeFragment++;
                        p->totalFragment++;
                        if (e->biome == VIOLENCIA)
                            cm->heaven = true;
                    }
                    p->hitEnemy = true;
                    p->biomeCoins += PACMAN_KILL_COINS;
                    p->totalCoins += PACMAN_KILL_COINS;
                    p->life = min(p->life + PACMAN_KILL_HEALTH, START_LIFE);
                    e->free(e);
                    enemies->removeNode(enemies, node);
                    return true;
                } else {
                    p->life -= ENEMY_DAMAGE;
                    p->damaged = true;
                    if (!e->isBoss) e->needToTeleport = true;
                }
                return false;
            }
        }
    }
    return false;
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DO INIMIGO
//===============================================================

static inline void updateEnemy(ChunkManager* cm, Node* node, LinkedList* list, Player* p, ArrayList* changedChunk, LinkedList* firedCells, LinkedList* tentacleCells) {
    Enemy* e = node->data;
    if (e->isBoss && p->effects.freezeTime.duration == 0)
        bossMecanics(cm, e, firedCells, tentacleCells);
    if (handlePlayerEnemyColision(cm, node, list, p)) return;
    updateEnemyMovement(cm, e, p);
    updateEnemyChunk(cm, node, list, changedChunk);
}

static inline void updateEnemies(Map* this) {
    Player* p = this->player;
    ChunkManager* cm = this->manager;

    ArrayList* changedChunk = this->changedChunk;
    changedChunk->clear(changedChunk);

    mapDistancePlayer(this);

    Chunk** adjacents = this->manager->adjacents;

    for (int i = 0; i < 9; i++) {
        Chunk* chunk = adjacents[CLOSER_IDX[i]];
        if (!chunk) continue;
        LinkedList* enemies = chunk->enemies;
        Node* cur = enemies->head;
        while (cur != NULL) {
            Node* next = cur->next;
            Enemy* e = cur->data;
            if (e->changedChunk) break;
            updateEnemy(cm, cur, enemies, p, changedChunk, this->firedCells, this->tentacleCells);
            cur = next;
        }
    }

    for (int i = 0; i < changedChunk->length; i++) {
        Enemy* e = changedChunk->data[i];
        e->changedChunk = false;
    }
}

//===============================================================
//  FUNÇÃO DE ATUALIZAÇÃO DO MAPA
//===============================================================

static void updateTime(Map* this, float deltaTime) {
    if (this->manager->heaven){
        this->biomeTime = 0;
        return;
    }
    if (this->player->effects.freezeTime.duration > 0) return;
    this->biomeTime += deltaTime;
    this->player->totalTime += deltaTime;

    if (this->biomeTime < BIOME_DEGEN_START_TIME) return;
    this->manager->degenerated = this->player->biome;
}

static void inline removeBossMecanics(ChunkManager* cm, LinkedList* firedCells, LinkedList* tentacleCells) {
    while (firedCells->length > BOSS_FIRE_QUANTITY) {
        Cell* cell = firedCells->removeFirst(firedCells);
        if (cell->biome >= HERESIA && rand() & 1) {
            cell->type = CELL_DEGENERATED_1;
        } else {
            cell->type = CELL_EMPTY;
        }
    }

    while (tentacleCells->length > BOSS_TENTACLE_QUANTITY) {
        Cell* cell = tentacleCells->removeFirst(tentacleCells);
        int prob = rand() % 100;
        if (prob < 70)
            cell->type = CELL_EMPTY;
        else if (prob < 96)
            cell->type = CELL_COIN;
        else if (prob < 97)
            cell->type = CELL_FREEZE_TIME;
        else if (prob < 98)
            cell->type = CELL_REGENERATION;
        else if (prob < 99)
            cell->type = CELL_INVISIBILITY;
        else if (prob < 100)
            cell->type = CELL_FRUIT;
    }

    if (cm->heaven) {
        while (firedCells->head) {
            Cell* cell = firedCells->removeFirst(firedCells);
            cell->type = CELL_HEAVEN;
        }

        while (tentacleCells->head) {
            Cell* cell = tentacleCells->removeFirst(tentacleCells);
            cell->type = CELL_HEAVEN;
        }
    }
}

//===============================================================
//  FUNÇÕES DE GERAÇÃO DO PORTAL NO FIM DO JOGO
//===============================================================

static inline void generatePortal(ChunkManager* cm) {
    if (cm->portal) return;
    int idx = rand() & 7;
    Chunk** adjacents = cm->adjacents;
    Chunk* chunk;

    for (int i = 0; i < 8; i++) {
        chunk = adjacents[ADJACENT_IDX[idx]];
        if (chunk != NULL) break;
        idx = (idx + 1) & 7;
    }

    int cx = rand() % 7 + 5;
    int cy = rand() % 7 + 5;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            Cell* cell = chunk->cellAt(chunk, cx + j, cy + i);
            cell->type = CELL_PORTAL;
            cell->biome = VIOLENCIA;
        }
    }
    cm->portal = true;
}

//===============================================================
//  MÉTODO DA CLASSE DE ATUALIZAÇÃO
//===============================================================

static void update(Map* this, Controller* controller, float deltaTime) {
    ChunkManager* cm = this->manager;
    cm->updateChunks(cm);
    updatePlayer(this, controller->input);
    updateEnemies(this);

    removeBossMecanics(cm, this->firedCells, this->tentacleCells);

    if (cm->heaven) generatePortal(cm);

    updateTime(this, deltaTime);

    this->updateCount++;
}

//===============================================================
//  MÉTODO DA CLASSE DE REINÍCIO
//===============================================================

static void restart(Map* this) {
    this->lastScore.totalCoins = this->player->totalCoins;
    this->lastScore.totalFragments = this->player->totalFragment;
    this->lastScore.totalTime = this->player->totalTime;
    this->player->restart(this->player);
    int biomeCols = this->manager->biomeCols;
    int rows = this->manager->rows;
    this->biomeTime = 0.0f;
    this->updateCount = 0;
    this->manager->free(this->manager);
    this->manager = new_ChunkManager(biomeCols, rows, this->player);
}


//===============================================================
//  MÉTODO DA CLASSE DE LIBERAÇÃO DE MEMÓRIA
//===============================================================

static void _free(Map* this) {
    this->manager->free(this->manager);
    Player* p = this->player;
    p->free(p);
    this->changedChunk->free(this->changedChunk);
    this->firedCells->free(this->firedCells);
    this->tentacleCells->free(this->tentacleCells);

    free(this);
}

//===============================================================
//  CONSTRUTOR DA CLASSE
//===============================================================

Map* new_Map(int biomeCols, int chunkRows, int spawnX, int spawnY) {
    Map* this = malloc(sizeof(Map));

    this->updateCount = 0;
    this->player = new_Player(spawnX, spawnY);

    this->changedChunk = new_ArrayList();

    this->firedCells = new_LinkedList();
    this->tentacleCells = new_LinkedList();

    this->biomeTime = 0.0f;

    this->manager = new_ChunkManager(biomeCols, chunkRows, this->player);

    this->update = update;
    this->restart = restart;
    this->free = _free;
    return this;
}
