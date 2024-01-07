#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "maps.h"

void extractUsernameAndPassword(unsigned char *message, char **username, char **password) {


    // Find the position of the '*' separator
    unsigned char *separator = strchr((char *)message + 1, '*');

    if (separator == NULL) {
        fprintf(stderr, "Invalid message format. Missing separator '*'.\n");
        *username = NULL;
        *password = NULL;
        return;
    }

    // Calculate the lengths of the username and password
    size_t usernameLength = separator - (message + 1);
    size_t passwordLength = strlen((char *)separator + 1);

    // Allocate memory for username and password
    *username = (char *)malloc(usernameLength + 1);
    *password = (char *)malloc(passwordLength + 1);

    // Copy username and password
    memcpy(*username, message + 2, usernameLength-1);
    (*username)[usernameLength] = '\0';

    memcpy(*password, separator + 2, passwordLength-1);
    (*password)[passwordLength] = '\0';
}

void initTextbox(Textbox *textbox, int x, int y, int w, int h) {
    strcpy(textbox->text, " \0");
    textbox->cursorPosition = 0;
    textbox->boxRect = (SDL_Rect){x, y, w, h};
    textbox->hasFocus = false;
}

void initButton(Button *button, int x, int y, int w, int h, const char *text) {
    strcpy(button->text, text);
    button->boxRect = (SDL_Rect){x, y, w, h};
    button->hasFocus = false;
}

int isValidPosition(int newX, int newY) {
    if (newX >= 0 && newX < 12 && newY >= 0 && newY < 12) {
        if (single_map_1[newY][newX] != 1 && single_map_1[newY][newX] != 2) {
            return 1; // The position is valid (true)
        }
    }
    return 0; // The position is not valid (false)
}

void handleKeyboardEvent(SDL_Event *e, Textbox *textbox) {
    if (textbox->hasFocus){
        if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_BACKSPACE && strlen(textbox->text) > 1) {
                textbox->text[strlen(textbox->text) - 1] = '\0';
            } else if (e->key.keysym.sym == SDLK_RETURN) {

            }
        } else if (e->type == SDL_TEXTINPUT) {
            strcat(textbox->text, e->text.text);
        }
    }
}

void setFocusOnTextbox(Textbox *textbox, int mouseX, int mouseY) {
    if (mouseX >= textbox->boxRect.x && mouseX <= textbox->boxRect.x + textbox->boxRect.w &&
        mouseY >= textbox->boxRect.y && mouseY <= textbox->boxRect.y + textbox->boxRect.h) {
        textbox->hasFocus = true;  // Mouse click is inside the textbox
    } else {
        textbox->hasFocus = false; // Mouse click is outside the textbox
    }
}




// char* generate_star_string(int n) {
//     if (n <= 0) {
//         return NULL;
//     }

//     char* result = (char*)malloc(n + 1);  // +1 for the null terminator
//     if (result == NULL) {
//         fprintf(stderr, "Memory allocation failed\n");
//         exit(1);
//     }

//     result[0] = ' ';
//     for (int i = 1; i < n; i++) {
//         result[0] = 'L';
//     }
//     result[n] = '\0';  // Null-terminate the string

//     return result;
// }
#endif