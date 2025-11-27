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

static int KEYS[127];

static void loadKeys() {
    for (int i = 0; i < 127; i++) {
        KEYS[i] = -1;
    }

    KEYS[KEY_W] = KEYS[GAMEPAD_BUTTON_LEFT_FACE_UP] = 0;
    KEYS[KEY_S] = KEYS[GAMEPAD_BUTTON_LEFT_FACE_DOWN] = 1;
    KEYS[KEY_D] = KEYS[GAMEPAD_BUTTON_LEFT_FACE_RIGHT] = 2;
    KEYS[KEY_A] = KEYS[GAMEPAD_BUTTON_LEFT_FACE_LEFT] = 3;
    KEYS[KEY_SPACE] = KEYS[GAMEPAD_BUTTON_RIGHT_TRIGGER_2] = 4;
    KEYS[KEY_Q] = KEYS[GAMEPAD_BUTTON_LEFT_TRIGGER_2] = 5;
    KEYS[KEY_I] = KEYS[GAMEPAD_BUTTON_RIGHT_FACE_UP] = 6;
    KEYS[KEY_K] = KEYS[GAMEPAD_BUTTON_RIGHT_FACE_DOWN] = 7;
    KEYS[KEY_L] = KEYS[GAMEPAD_BUTTON_RIGHT_FACE_RIGHT] = 8;
    KEYS[KEY_J] = KEYS[GAMEPAD_BUTTON_RIGHT_FACE_LEFT] = 9;
    KEYS[KEY_U] = KEYS[GAMEPAD_BUTTON_LEFT_TRIGGER_1] = 10;
    KEYS[KEY_O] = KEYS[GAMEPAD_BUTTON_RIGHT_TRIGGER_1] = 11;
}

static void addCheat(char* cheat) {
    int rootIdx = KEYS[*cheat];
    TreeNode* node = new_TreeNode();
    cheats[rootIdx] = node;
    for (char* c = cheat + 1; *c; c++) {
        int nodeIdx = KEYS[*c];
        int code = nodeIdx != -1 ? 0 : *c - '0';
        if (code) {
            node->code = code;
            return;
        }
        TreeNode* proxNode = new_TreeNode();
        node->nodes[nodeIdx] = proxNode;
        node = proxNode;
    }
}

void loadCheats() {
    loadKeys();
    FILE* file = fopen("assets/cheats.txt", "r");
    static char buffer[1000];

    while (fgets(buffer, sizeof(buffer), file)) {
        addCheat(buffer);
    }

    fclose(file);
}

void unloadCheats() {

}

int hasCheat(InputBuffer* inputs) {
    if (inputs->length == 0) return -1;
    int idx = KEYS[inputs->data[0]];
    TreeNode* node = cheats[idx];
    if (node == NULL) return -1;
    for (int i = 1; i < inputs->length; i++){
        node = node->nodes[KEYS[inputs->data[i]]];
        if (node == NULL) return -1;
        if (node->code > 0) return node->code;
    }
    if (node != NULL) return 0;
    return -1;
}