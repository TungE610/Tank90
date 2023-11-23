#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL.h>

struct Tank {
    int x;
    int y;
};

struct Bullet {
    SDL_Rect position;
    int face;
    int player_id;
};

#endif
