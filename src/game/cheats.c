#include "cheats.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//===============================================================
//  STRUCT DO NÓ DA ÁRVORE DE CHEATS
//===============================================================

typedef struct TreeNode {
    struct TreeNode* nodes[12];
    int code;
} TreeNode;

//===============================================================
//  ÁRVORES DE CHEATS
//===============================================================

TreeNode* roots[12] = { 0 };

//===============================================================
//  CONSTRUTOR DE UM NÓ DA ÁRVORE
//===============================================================

static inline TreeNode* new_TreeNode() {
    return calloc(1, sizeof(TreeNode));
}

//===============================================================
//  HASH PERFEITO
//===============================================================

static int KEYS[512];

//===============================================================
//  CARREGAR TABELA
//===============================================================

static void loadKeys() {
    for (int i = 0; i < 512; i++) {
        KEYS[i] = -1;
    }

    KEYS[KEY_W] = KEYS[GAMEPAD_BUTTON_LEFT_FACE_UP] = KEYS[KEY_UP] = 0;
    KEYS[KEY_S] = KEYS[GAMEPAD_BUTTON_LEFT_FACE_DOWN] = KEYS[KEY_DOWN] = 1;
    KEYS[KEY_D] = KEYS[GAMEPAD_BUTTON_LEFT_FACE_RIGHT] = KEYS[KEY_RIGHT] = 2;
    KEYS[KEY_A] = KEYS[GAMEPAD_BUTTON_LEFT_FACE_LEFT] = KEYS[KEY_LEFT] = 3;
    KEYS[KEY_SPACE] = KEYS[GAMEPAD_BUTTON_RIGHT_TRIGGER_2] = 4;
    KEYS[KEY_Q] = KEYS[GAMEPAD_BUTTON_LEFT_TRIGGER_2] = 5;
    KEYS[KEY_I] = KEYS[GAMEPAD_BUTTON_RIGHT_FACE_UP] = 6;
    KEYS[KEY_K] = KEYS[GAMEPAD_BUTTON_RIGHT_FACE_DOWN] = 7;
    KEYS[KEY_L] = KEYS[GAMEPAD_BUTTON_RIGHT_FACE_RIGHT] = 8;
    KEYS[KEY_J] = KEYS[GAMEPAD_BUTTON_RIGHT_FACE_LEFT] = 9;
    KEYS[KEY_U] = KEYS[GAMEPAD_BUTTON_LEFT_TRIGGER_1] = 10;
    KEYS[KEY_O] = KEYS[GAMEPAD_BUTTON_RIGHT_TRIGGER_1] = 11;
}

//===============================================================
//  FUNÇÃO AUXILIAR DE STRING
//===============================================================

static int getNumber(char* str) {
    char c;
    int number = 0;
    while ((c = *str++) != ' ' && c != 0) {
        number *= 10;
        number += (c - '0') % 10;
    }
    return number;
}

//===============================================================
//  ADICIONAR RAMIFICAÇÃO DO CHEAT VIA STRING
//===============================================================

static void addCheat(char* cheat) {
    int rootIdx = KEYS[(int)(*cheat)];
    if (roots[rootIdx] == NULL) {
        roots[rootIdx] = new_TreeNode();
    }
    TreeNode* node = roots[rootIdx];
    for (char* c = cheat + 2; *c; c += 2) {
        int nodeIdx = KEYS[(int)(*c)];
        if (node->nodes[nodeIdx] == NULL) {
            node->nodes[nodeIdx] = new_TreeNode();
        }
        node = node->nodes[nodeIdx];
        if (*(c + 1) == ':') {
            node->code = getNumber(c + 2);
            return;
        }
    }
}

//===============================================================
//  GERAR E CARREGAR A ÁRVORE COMPLETA
//===============================================================

void loadCheats() {
    loadKeys();
    FILE* file = fopen("data/cheats.txt", "r");
    static char buffer[1000];

    while (fgets(buffer, sizeof(buffer), file)) {
        addCheat(buffer);
    }

    fclose(file);
}

//===============================================================
//  PONTEIRO PARA O RAMO DO INPUT
//===============================================================

static TreeNode* node = NULL;

//===============================================================
//  FUNÇÃO QUE VERIFICA E ATUALIZA O PONTEIRO
//===============================================================

inline int hasCheat(int code) {
    int idx = KEYS[code];

    if (idx == -1) {
        node = NULL;
        return 0;
    }

    if (node == NULL) node = roots[idx]; // Sair da raiz
    else node = node->nodes[idx]; // Avançar um galho

    if (node) return node->code;
    return 0;
}

//===============================================================
//  LEVAR O PONTEIRO PARA A ORIGEM
//===============================================================

void resetCheatPointer() {
    node = NULL;
}

//===============================================================
//  LIBERAR MEMÓRIA DE UM NÓ
//===============================================================

static void freeRecursive(TreeNode* node) {
    for (int i = 0; i < 12; i++) {
        if (node->nodes[i] == NULL) continue;
        freeRecursive(node->nodes[i]);
    }
    free(node);
}

//===============================================================
//  DESCARREGAR ÁRVORE
//===============================================================

inline void unloadCheats() {
    for (int i = 0; i < 12; i++) {
        if (roots[i] == NULL) continue;
        freeRecursive(roots[i]);
    }
}