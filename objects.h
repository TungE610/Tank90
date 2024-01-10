#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL.h>
#include "authenticate.h"

#define BUFF_SIZE 1024

typedef struct{
    int id;
    int first_player_id;
    int second_player_id;
    int status;
} Room;

struct Player {
    int id;
    int system_id;
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    SDL_Rect position;
    int left, right, up, down;
    int can_jump, y_speed;
    int face, shoot, reloading;
    int left_key, right_key, up_key, down_key, attack_key;
    int scores;
    int is_choosing_game_mode;
    int game_mode;
    int is_waiting_other_player;
    struct sockaddr_in address;
    int socket;
    singleLinkedList *list;
};

typedef struct {
    char username[BUFF_SIZE];
    int score;
} TopPlayer;

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// enum Direction direction = LEFT;

typedef struct {
    int position_x;
    int position_y;
    int player_id;
    int direction;
    int is_active;
} Bullet;

typedef struct {
    // int position_x;
    // int position_y;
    int id;
    // int direction;
    int blood;
    int exist;
} Enermy;

typedef struct {
    char text[256];
    int cursorPosition;
    SDL_Rect boxRect; // Position and size of the textbox
    bool hasFocus;
} Textbox;

typedef struct {
    char text[256];
    SDL_Rect boxRect; // Position and size of the button
    bool hasFocus;
} Button;

#endif
