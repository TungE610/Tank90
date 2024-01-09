#ifndef TEXTURES_H
#define TEXTURES_H

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "surfaces.h"

SDL_Texture *tank = NULL;
SDL_Texture *block = NULL;
SDL_Texture *iron = NULL;
SDL_Texture *grass = NULL;
SDL_Texture *water = NULL;
SDL_Texture *meUp = NULL;
SDL_Texture *meDown = NULL;
SDL_Texture *meLeft = NULL;
SDL_Texture *meRight = NULL;
SDL_Texture *enermy_1_right = NULL;
SDL_Texture *enermy_1_left = NULL;
SDL_Texture *enermy_1_up = NULL;
SDL_Texture *enermy_1_down = NULL;
SDL_Texture *enermy_2_right = NULL;
SDL_Texture *enermy_2_left = NULL;
SDL_Texture *enermy_2_up = NULL;
SDL_Texture *enermy_2_down = NULL;
SDL_Texture *enermy_3_right = NULL;
SDL_Texture *enermy_3_left = NULL;
SDL_Texture *enermy_3_up = NULL;
SDL_Texture *enermy_3_down = NULL;
SDL_Texture *enermy_4_right = NULL;
SDL_Texture *enermy_4_left = NULL;
SDL_Texture *enermy_4_up = NULL;
SDL_Texture *enermy_4_down = NULL;
SDL_Texture *friendTank;
SDL_Texture *myTank;
SDL_Texture *myTank_2;

SDL_Texture *loginBigTextTexture;
SDL_Texture *registerBigTextTexture;
SDL_Texture *roomsBigTextTexture;
SDL_Texture *usernameTextTexture;
SDL_Texture *passwordTextTexture;
SDL_Texture *retypePasswordTextTexture;
SDL_Texture *chooseModeTextTexture;
SDL_Texture *singleModeTextTexture;
SDL_Texture *dualModeTextTexture;
SDL_Texture *showPasswordTexture;
SDL_Texture *hidePasswordTexture;
SDL_Texture *hittedTexture;
SDL_Texture *single_1_enermyTexture[4];
SDL_Texture *bullet_up;
SDL_Texture *bullet_down;
SDL_Texture *bullet_right;
SDL_Texture *bullet_left;
SDL_Texture *bulletTexture[20];

void loadTextures(SDL_Renderer *renderer) {
    tank = IMG_LoadTexture(renderer, "images/player_green_up.png");
    block = IMG_LoadTexture(renderer, "images/block.png");
    iron = IMG_LoadTexture(renderer, "images/iron.png");
    grass = IMG_LoadTexture(renderer, "images/grass.png");
    water = IMG_LoadTexture(renderer, "images/water.png");
    meUp = IMG_LoadTexture(renderer, "images/me_up.png");
    meRight = IMG_LoadTexture(renderer, "images/me_right.png");
    meLeft = IMG_LoadTexture(renderer, "images/me_left.png");
    meDown = IMG_LoadTexture(renderer, "images/me_down.png");
    enermy_1_right = IMG_LoadTexture(renderer, "images/enermy_right.png");
    enermy_1_left = IMG_LoadTexture(renderer, "images/enermy_left.png");
    enermy_1_up = IMG_LoadTexture(renderer, "images/enermy_up.png");
    enermy_1_down = IMG_LoadTexture(renderer, "images/enermy_down.png");
    enermy_2_right = IMG_LoadTexture(renderer, "images/enermy_right.png");
    enermy_2_left = IMG_LoadTexture(renderer, "images/enermy_left.png");
    enermy_2_up = IMG_LoadTexture(renderer, "images/enermy_up.png");
    enermy_2_down = IMG_LoadTexture(renderer, "images/enermy_down.png");
    enermy_3_right = IMG_LoadTexture(renderer, "images/enermy_right.png");
    enermy_3_left = IMG_LoadTexture(renderer, "images/enermy_left.png");
    enermy_3_up = IMG_LoadTexture(renderer, "images/enermy_up.png");
    enermy_3_down = IMG_LoadTexture(renderer, "images/enermy_down.png");
    enermy_4_right = IMG_LoadTexture(renderer, "images/enermy_right.png");
    enermy_4_left = IMG_LoadTexture(renderer, "images/enermy_left.png");
    enermy_4_up = IMG_LoadTexture(renderer, "images/enermy_up.png");
    enermy_4_down = IMG_LoadTexture(renderer, "images/enermy_down.png");
    showPasswordTexture = IMG_LoadTexture(renderer, "images/show_password.png");
    hidePasswordTexture = IMG_LoadTexture(renderer, "images/hide_password.png");
    hittedTexture = IMG_LoadTexture(renderer, "images/hitted.png");
    single_1_enermyTexture[0] = IMG_LoadTexture(renderer, "images/enermy_left.png");
    single_1_enermyTexture[1] = IMG_LoadTexture(renderer, "images/enermy_left.png");
    single_1_enermyTexture[2] = IMG_LoadTexture(renderer, "images/enermy_left.png");
    single_1_enermyTexture[3] = IMG_LoadTexture(renderer, "images/enermy_left.png");
    bullet_up = IMG_LoadTexture(renderer, "images/bullet_up.png");
    bullet_down = IMG_LoadTexture(renderer, "images/bullet_down.png");
    bullet_right = IMG_LoadTexture(renderer, "images/bullet_right.png");
    bullet_left = IMG_LoadTexture(renderer, "images/bullet.png");

    friendTank = meUp;
    myTank = meUp;
    myTank_2 = meUp;

    loginBigTextTexture = SDL_CreateTextureFromSurface(renderer, loginBigTextSurface); 
    registerBigTextTexture = SDL_CreateTextureFromSurface(renderer, registerBigText); 
    roomsBigTextTexture = SDL_CreateTextureFromSurface(renderer, roomsBigText);
    usernameTextTexture = SDL_CreateTextureFromSurface(renderer, usernameText); 
    passwordTextTexture = SDL_CreateTextureFromSurface(renderer, passwordText);
    retypePasswordTextTexture = SDL_CreateTextureFromSurface(renderer, retypePasswordText);
    chooseModeTextTexture = SDL_CreateTextureFromSurface(renderer, chooseModeText);  
    singleModeTextTexture = SDL_CreateTextureFromSurface(renderer, singleModeText);      
    dualModeTextTexture = SDL_CreateTextureFromSurface(renderer, dualModeText);      
}

#endif
