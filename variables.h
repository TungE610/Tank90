#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "objects.h"

SDL_Rect controlRect = { 240, 440, 40, 40 };

int vertical_controller = 88;
int hozirontal_controller = 32;

int vertical_controller_friend = 88;
int hozirontal_controller_friend = 64;

int single_mode_postion_x = 4;
int single_mode_postion_y = 11;

int dual_mode_position_x, dual_mode_position_y;
int dual_mode_position_x_friend, dual_mode_position_y_friend;

int tankX = 220;
int tankY = 245;
int tankWidth = 30;
int tankHeight = 30;


// login page ----------------------------------
char login_message[BUFF_SIZE];
char register_message[BUFF_SIZE];

Textbox usernameLoginTextbox;
Textbox passwordLoginTextbox;

Textbox usernameRegisterTextbox;
Textbox passwordRegisterTextbox;
Textbox retypePasswordRegisterTextbox;

Button loginButton;
//register page---------------------------------------------------
Button regiterButton;
int showPassword = 0;
int showRetypePassword = 0;

#endif
