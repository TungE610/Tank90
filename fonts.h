#ifndef FONTS_H
#define FONTS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "colors.h"

TTF_Font *BIG_FONT;
TTF_Font *MEDIUM_FONT;
TTF_Font *SMALL_FONT;
TTF_Font *TINY_FONT;
TTF_Font *MINI_FONT;

void initFonts() {

    BIG_FONT = TTF_OpenFont("./resources/font/ARCADE.TTF", 50);  // Adjust font size as needed
    MEDIUM_FONT = TTF_OpenFont("./resources/font/ARCADE.TTF", 35);  // Adjust font size as needed
    SMALL_FONT = TTF_OpenFont("./resources/font/ARCADE.TTF", 30);  // Adjust font size as needed
    TINY_FONT = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);  // Adjust font size as needed
    MINI_FONT = TTF_OpenFont("./resources/font/ARCADE.TTF", 13);  // Adjust font size as neededc
}

#endif
