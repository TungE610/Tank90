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

int myId = 0;
int in_room_id = 0;
SDL_Rect single_controlRect = { 240, 440, 40, 40 };
SDL_Rect single_controlRect_2 = { 240, 440, 40, 40 };
SDL_Rect single_controlRect_3 = { 240, 440, 40, 40 };
SDL_Rect single_controlRect_4 = { 240, 440, 40, 40 };

int vertical_controller = 88;
int hozirontal_controller = 32;

int single_mode_postion_x = 4;
int single_mode_postion_y = 11;
// single 1
SDL_Rect single_1_enermy_react[4];
int single_1_enermy_vertical_controller[4];
int single_1_enermy_hozirontal_controller[4];
int single_1_mode_enermy_postion_x[4];
int single_1_mode_enermy_postion_y[4];
int single_1_consecutive_count[4];
int single_1_direction[4];
int single_1_previousDirection[4];
Bullet single_1_bullet[20];
Enermy single_1_enermies[4];
SDL_Rect single_1_bulletRect[20];
int single_scores = 0;
int removed[4];
int removeNum = 0;

void initValueForSingle1() {
    removeNum = 0;
    single_1_enermy_react[0] = (SDL_Rect){ 80, 0, 40, 40 };
    single_1_enermy_react[1] = (SDL_Rect){ 520, 200, 40, 40 };
    single_1_enermy_react[2] = (SDL_Rect){ 520, 0, 40, 40 };
    single_1_enermy_react[3] = (SDL_Rect){ 80, 200, 40, 40 };

    single_1_enermy_hozirontal_controller[0] = 0;
    single_1_enermy_hozirontal_controller[1] = 440;
    single_1_enermy_hozirontal_controller[2] = 440;
    single_1_enermy_hozirontal_controller[3] = 0;

    single_1_enermy_vertical_controller[0] = 0;
    single_1_enermy_vertical_controller[1] = 200;
    single_1_enermy_vertical_controller[2] = 0;
    single_1_enermy_vertical_controller[3] = 200;

    single_1_mode_enermy_postion_x[0] = 0;
    single_1_mode_enermy_postion_x[1] = 11;
    single_1_mode_enermy_postion_x[2] = 11;
    single_1_mode_enermy_postion_x[3] = 0;

    single_1_mode_enermy_postion_y[0] = 0;
    single_1_mode_enermy_postion_y[1] = 5;
    single_1_mode_enermy_postion_y[2] = 0;
    single_1_mode_enermy_postion_y[3] = 5;

    for (int i = 0; i < 4;i ++) {
        single_1_direction[i] = 0;
        single_1_consecutive_count[i] = 0;
        single_1_previousDirection[i] = 0;
    }

    for (int i = 0; i < 20; i++) {
        single_1_bullet[i].position_x = hozirontal_controller;
        single_1_bullet[i].position_y = vertical_controller;
        single_1_bullet[i].is_active = 0;
        single_1_bulletRect[i] = (SDL_Rect){ single_controlRect.x, single_controlRect.y, 40, 40 };
    }

    for (int i = 0; i < 4; i ++) {
        single_1_enermies[i].id = i;
        single_1_enermies[i].id = 1;
        single_1_enermies[i].exist = 1;
    }
}
// -------------------------------------single 2---------------------------

int vertical_controller_2 = 88;
int hozirontal_controller_2 = 32;

int single_mode_postion_x_2 = 4;
int single_mode_postion_y_2 = 11;

int vertical_controller_friend = 88;
int hozirontal_controller_friend = 64;


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
