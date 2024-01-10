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
int isFirstUserInRoom = 0;

SDL_Rect single_controlRect = { 240, 440, 40, 40 };
SDL_Rect single_controlRect_2 = { 240, 440, 40, 40 };
SDL_Rect single_controlRect_3 = { 240, 440, 40, 40 };

SDL_Rect dual_controlRect = { 240, 440, 40, 40 };
SDL_Rect dual_friendRect = { 400, 440, 40, 40 };

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

int removed_2[5];
int removeNum_2 = 0;

int removed_3[6];
int removeNum_3 = 0;

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
        single_1_enermies[i].blood = 1;
        single_1_enermies[i].exist = 1;
    }
}
// -------------------------------------single 2---------------------------

Enermy single_2_enermies[5];
Bullet single_2_bullet[30];
SDL_Rect single_2_bulletRect[30];

int single_2_direction[5];
int single_2_consecutive_count[5];
int single_2_previousDirection[5];

int vertical_controller_2 = 88;
int hozirontal_controller_2 = 32;

int single_mode_postion_x_2 = 4;
int single_mode_postion_y_2 = 11;

int single_2_mode_enermy_postion_x[5];
int single_2_mode_enermy_postion_y[5];
int single_2_enermy_hozirontal_controller[5];
int single_2_enermy_vertical_controller[5];

SDL_Rect single_2_enermy_react[5];

void initValueForSingle2() {
    removeNum_2 = 0;
    single_2_enermy_react[0] = (SDL_Rect){ 80, 0, 40, 40 };
    single_2_enermy_react[1] = (SDL_Rect){ 520, 200, 40, 40 };
    single_2_enermy_react[2] = (SDL_Rect){ 520, 0, 40, 40 };
    single_2_enermy_react[3] = (SDL_Rect){ 80, 200, 40, 40 };
    single_2_enermy_react[4] = (SDL_Rect){ 320, 0, 40, 40 };

    single_2_enermy_hozirontal_controller[0] = 0;
    single_2_enermy_hozirontal_controller[1] = 440;
    single_2_enermy_hozirontal_controller[2] = 440;
    single_2_enermy_hozirontal_controller[3] = 0;
    single_2_enermy_hozirontal_controller[4] = 240;

    single_2_enermy_vertical_controller[0] = 0;
    single_2_enermy_vertical_controller[1] = 200;
    single_2_enermy_vertical_controller[2] = 0;
    single_2_enermy_vertical_controller[3] = 200;
    single_2_enermy_vertical_controller[4] = 0;

    single_2_mode_enermy_postion_x[0] = 0;
    single_2_mode_enermy_postion_x[1] = 11;
    single_2_mode_enermy_postion_x[2] = 11;
    single_2_mode_enermy_postion_x[3] = 0;
    single_2_mode_enermy_postion_x[4] = 6;

    single_2_mode_enermy_postion_y[0] = 0;
    single_2_mode_enermy_postion_y[1] = 5;
    single_2_mode_enermy_postion_y[2] = 0;
    single_2_mode_enermy_postion_y[3] = 5;
    single_2_mode_enermy_postion_y[4] = 0;

    for (int i = 0; i < 5;i ++) {
        single_2_direction[i] = 0;
        single_2_consecutive_count[i] = 0;
        single_2_previousDirection[i] = 0;
    }

    for (int i = 0; i < 30; i++) {
        single_2_bullet[i].position_x = hozirontal_controller;
        single_2_bullet[i].position_y = vertical_controller;
        single_2_bullet[i].is_active = 0;
        single_2_bulletRect[i] = (SDL_Rect){ single_controlRect_2.x, single_controlRect_2.y, 40, 40 };
    }

    for (int i = 0; i < 5; i ++) {
        single_2_enermies[i].id = i;
        single_2_enermies[i].blood = 1;
        single_2_enermies[i].exist = 1;
    }
}

// -------------------------------------single 3---------------------------

Enermy single_3_enermies[6];
Bullet single_3_bullet[30];
SDL_Rect single_3_bulletRect[30];

int single_3_direction[6];
int single_3_consecutive_count[6];
int single_3_previousDirection[6];

int vertical_controller_3 = 88;
int hozirontal_controller_3 = 32;

int single_mode_postion_x_3 = 4;
int single_mode_postion_y_3 = 11;

int single_3_mode_enermy_postion_x[6];
int single_3_mode_enermy_postion_y[6]; 
int single_3_enermy_hozirontal_controller[6];
int single_3_enermy_vertical_controller[6];

SDL_Rect single_3_enermy_react[6];

void initValueForSingle3() {
    removeNum_3 = 0;
    single_3_enermy_react[0] = (SDL_Rect){ 80, 0, 40, 40 };
    single_3_enermy_react[1] = (SDL_Rect){ 520, 200, 40, 40 };
    single_3_enermy_react[2] = (SDL_Rect){ 520, 0, 40, 40 };
    single_3_enermy_react[3] = (SDL_Rect){ 80, 200, 40, 40 };
    single_3_enermy_react[4] = (SDL_Rect){ 320, 0, 40, 40 };
    single_3_enermy_react[5] = (SDL_Rect){ 280, 160, 40, 40 };

    single_3_enermy_hozirontal_controller[0] = 0;
    single_3_enermy_hozirontal_controller[1] = 440;
    single_3_enermy_hozirontal_controller[2] = 440;
    single_3_enermy_hozirontal_controller[3] = 0;
    single_3_enermy_hozirontal_controller[4] = 240;
    single_3_enermy_hozirontal_controller[5] = 200;

    single_3_enermy_vertical_controller[0] = 0;
    single_3_enermy_vertical_controller[1] = 200;
    single_3_enermy_vertical_controller[2] = 0;
    single_3_enermy_vertical_controller[3] = 200;
    single_3_enermy_vertical_controller[4] = 0;
    single_3_enermy_vertical_controller[5] = 160;

    single_3_mode_enermy_postion_x[0] = 0;
    single_3_mode_enermy_postion_x[1] = 11;
    single_3_mode_enermy_postion_x[2] = 11;
    single_3_mode_enermy_postion_x[3] = 0;
    single_3_mode_enermy_postion_x[4] = 6;
    single_3_mode_enermy_postion_x[5] = 5;

    single_3_mode_enermy_postion_y[0] = 0;
    single_3_mode_enermy_postion_y[1] = 5;
    single_3_mode_enermy_postion_y[2] = 0;
    single_3_mode_enermy_postion_y[3] = 5;
    single_3_mode_enermy_postion_y[4] = 0;
    single_3_mode_enermy_postion_y[5] = 4;

    for (int i = 0; i < 6;i ++) {
        single_3_direction[i] = 0;
        single_3_consecutive_count[i] = 0;
        single_3_previousDirection[i] = 0;
    }

    for (int i = 0; i < 30; i++) {
        single_3_bullet[i].position_x = hozirontal_controller;
        single_3_bullet[i].position_y = vertical_controller;
        single_3_bullet[i].is_active = 0;
        single_3_bulletRect[i] = (SDL_Rect){ single_controlRect_3.x, single_controlRect_3.y, 40, 40 };
    }

    for (int i = 0; i < 6; i ++) {
        single_3_enermies[i].id = i;
        single_3_enermies[i].blood = 2;
        single_3_enermies[i].exist = 1;
    }
}

int dual_control_vertical_controller = 88;
int dual_control_hozirontal_controller = 32;

int dual_control_vertical_controller_friend = 88;
int dual_control_hozirontal_controller_friend = 64;

int dual_mode_position_x = 4;
int dual_mode_position_y = 11;
int dual_mode_position_x_friend = 8;
int dual_mode_position_y_friend = 11;

Bullet dual_bullet[30];
SDL_Rect dual_bulletRect[30];

Bullet dual_bullet_friend[30];
SDL_Rect dual_bulletRect_friend[30];

void initValueForDual() {

    for (int i = 0; i < 30; i++) {
        dual_bullet[i].position_x = dual_control_hozirontal_controller;
        dual_bullet[i].position_y = dual_control_vertical_controller;
        dual_bullet[i].is_active = 0;
        dual_bulletRect[i] = (SDL_Rect){ dual_controlRect.x, dual_controlRect.y, 40, 40 };
    }
}

void initValueForDual_friend() {

    for (int i = 0; i < 30; i++) {
        dual_bullet_friend[i].position_x = dual_control_hozirontal_controller_friend;
        dual_bullet_friend[i].position_y = dual_control_vertical_controller_friend;
        dual_bullet_friend[i].is_active = 0;
        dual_bulletRect_friend[i] = (SDL_Rect){ dual_friendRect.x, dual_friendRect.y, 40, 40 };
    }
}

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
