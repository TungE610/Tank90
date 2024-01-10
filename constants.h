#include "stdbool.h"

#define MAX_PLAYERS 20
#define totalBullets 20
#define SERV_IP "127.0.0.1"
#define SERV_PORT 5500
#define TANK_WIDTH 30
#define TANK_HEIGHT 30
#define SCREEN_WIDTH 700
#define SCREEN_HEIGHT 540

enum AppState {
    MAIN_MENU,
    LOGIN,
    REGISTER,
    CHOOSE_MODE,
    PLAY_SINGLE_MAP_1,
    CHANGING_TO_SINGLE_MAP_2,
    PLAY_SINGLE_MAP_2,
    CHANGING_TO_SINGLE_MAP_3,
    PLAY_SINGLE_MAP_3,
    CHANGING_TO_SINGLE_MAP_4,
    PLAY_SINGLE_MAP_4,
    CHOOSE_ROOM,
    WAITING_OTHER,
    READY_TO_PLAY_DUAL,
    SELECT_DUAL_MAPS,
    PLAY_DUAL_GAME,
};

enum AppState state = MAIN_MENU; // Start in the main menu state
