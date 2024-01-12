#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "maps.h"

void extractUsernameAndPassword(const char *message, char *username, char *password) {
    // Find the position of the '*' character in the message
    const char *delimiter = strchr(message, '*');

    if (delimiter != NULL) {
        // Calculate the length of the username and password parts
        size_t usernameLength = delimiter - message;
        size_t passwordLength = strlen(delimiter + 1);

        // Copy the username and password parts into the provided buffers
        strncpy(username, message, usernameLength);
        strncpy(password, delimiter + 1, passwordLength);

        // Null-terminate the strings
        username[usernameLength] = '\0';
        password[passwordLength] = '\0';
    } else {
        // Handle the case where no '*' character is found in the message
        strcpy(username, "");
        strcpy(password, "");
    }
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

int round_integer_division(int numerator, int denominator) {
    if (denominator == 0) {
        printf("Không thể chia cho 0\n");
        return 0; // Hoặc giá trị khác tùy bạn
    }

    double result = (double)numerator / denominator; // Chuyển sang kiểu double trước khi chia
    return round(result); // Sử dụng hàm round để làm tròn
}


int isMouseClickOnTexture(int mouseX, int mouseY, SDL_Texture *texture, SDL_Rect renderQuad) {
    int texW, texH;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);

    return (mouseX >= renderQuad.x && mouseX <= renderQuad.x + texW && mouseY >= renderQuad.y && mouseY <= renderQuad.y + texH);
}

int compare_scores(const void *a, const void *b) {
    return (*(node**)b)->element.score - (*(node**)a)->element.score;
}


#endif