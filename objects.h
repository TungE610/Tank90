#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL.h>

struct Room {
    int id;
    int first_player;
    int second_player;
    int status;
};

struct Player {
    int id;
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
};

struct Bullet {
    SDL_Rect position;
    int face;
    int player_id;
};

#endif
