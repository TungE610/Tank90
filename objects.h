#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL.h>
#include "authenticate.h"

#define BUFF_SIZE 1024

struct Room {
    int id;
    int first_player;
    int second_player;
    int status;
};

struct Player {
    int id;
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    SDL_Rect position;
    int left, right, up, down;
    int can_jump, y_speed;
    int face, shoot, reloading;
    int left_key, right_key, up_key, down_key, attack_key;
    int kills, deaths;
    int is_choosing_game_mode;
    int game_mode;
    int is_waiting_other_player;
    struct sockaddr_in address;
    int socket;
    singleLinkedList *list;
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

typedef struct {
    int position_x;
    int position_y;
    int player_id;
    int direction;
    int is_active;
} Bullet;

typedef struct {
    int position_x;
    int position_y;
    int id;
    int direction;
    int blood;
    float movementTimer; // Add a movement timer
} Enermy;

#endif
