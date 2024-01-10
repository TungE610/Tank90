#ifndef SURFACES_H
#define SURFACES_H

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "colors.h"
#include "fonts.h"

SDL_Surface *loginBigTextSurface;
SDL_Surface *rankingBigTextSurface;
SDL_Surface *gameOverBigTextSurface;
SDL_Surface *wonBigTextSurface;
SDL_Surface *registerBigText;
SDL_Surface *roomsBigText;
SDL_Surface *usernameText;
SDL_Surface *retypePasswordText;
SDL_Surface *passwordText;
SDL_Surface *chooseModeText;
SDL_Surface *singleModeText;
SDL_Surface *dualModeText;
SDL_Surface *textSurface;
SDL_Surface *loginText;
SDL_Surface *registerText;

void loadSurface() {

    loginBigTextSurface = TTF_RenderText_Solid(BIG_FONT, "LOGIN", WHITE);
    rankingBigTextSurface = TTF_RenderText_Solid(BIG_FONT, "RANKING", WHITE);
    gameOverBigTextSurface = TTF_RenderText_Solid(BIG_FONT, "GAME OVER", WHITE);
    wonBigTextSurface = TTF_RenderText_Solid(BIG_FONT, "WON", WHITE);
    registerBigText = TTF_RenderText_Solid(BIG_FONT, "REGISTER", WHITE);
    roomsBigText = TTF_RenderText_Solid(BIG_FONT, "ROOMS", WHITE);
    usernameText = TTF_RenderText_Solid(TINY_FONT, "Username: ", WHITE);
    retypePasswordText = TTF_RenderText_Solid(TINY_FONT, "Retype Password: ", WHITE);
    passwordText = TTF_RenderText_Solid(TINY_FONT, "Password: ", WHITE);
    chooseModeText = TTF_RenderText_Solid(BIG_FONT, "Choose Mode", WHITE);
    singleModeText = TTF_RenderText_Solid(TINY_FONT, "1. Single", WHITE);
    dualModeText = TTF_RenderText_Solid(TINY_FONT, "2. Dual", WHITE);
    textSurface = TTF_RenderText_Solid(BIG_FONT, "TANK 90", WHITE);
    loginText = TTF_RenderText_Solid(TINY_FONT, "1. Login", WHITE);
    registerText = TTF_RenderText_Solid(TINY_FONT, "2. Register", WHITE);
}

#endif
