#include "cheats.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct TreeNode {
    struct TreeNode* nodes[12];
    int code;
} TreeNode;

TreeNode* cheats[12] = { 0 };

static TreeNode* new_TreeNode() {
    return calloc(1, sizeof(TreeNode));
}

static int KEYS[512];

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

static inline int getNumber(char* str){
    char c;
    int number = 0;
    while((c = *str++) != ' ' && c != 0){
        number *= 10;
        number += (c - '0') % 10;
    }
    return number;
}

static void addCheat(char* cheat) {
    int rootIdx = KEYS[(int)(*cheat)];
    if (cheats[rootIdx] == NULL) {
        cheats[rootIdx] = new_TreeNode();
    }
    TreeNode* node = cheats[rootIdx];
    for (char* c = cheat + 2; *c; c+=2) {
        int nodeIdx = KEYS[(int)(*c)];
        if (node->nodes[nodeIdx] == NULL) {
            node->nodes[nodeIdx] = new_TreeNode();
        }
        node = node->nodes[nodeIdx];
        if (*(c + 1) == ':'){
            node->code = getNumber(c + 2);
            return;
        }
    }
}

void loadCheats() {
    loadKeys();
    FILE* file = fopen("data/cheats.txt", "r");
    static char buffer[1000];

    while (fgets(buffer, sizeof(buffer), file)) {
        addCheat(buffer);
    }

    fclose(file);
}

static void freeRecursive(TreeNode* node) {
    for (int i = 0; i < 12; i++) {
        if (node->nodes[i] == NULL) continue;
        freeRecursive(node->nodes[i]);
    }
    free(node);
}

void unloadCheats() {
    for (int i = 0; i < 12; i++) {
        if (cheats[i] == NULL) continue;
        freeRecursive(cheats[i]);
    }
}

static TreeNode* node = NULL;

int hasCheat(int code) {
    int idx = KEYS[code];

    if (idx == -1) {
        node = NULL;
        return 0;
    }

    if (node == NULL) node = cheats[idx]; // Sair da raiz
    else node = node->nodes[idx]; // Avançar um galho

    if (node) return node->code;
    return 0;
}

void resetCheatPointer(){
    node = NULL;
}