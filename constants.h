#include "stdbool.h"

#define MAX_PLAYERS 20
#define totalBullets 20
#define SERV_IP "127.0.0.1"
#define SERV_PORT 5500
#define TANK_WIDTH 30
#define TANK_HEIGHT 30
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 540

enum AppState {
    MAIN_MENU,
    LOGIN,
    REGISTER,
    CHOOSE_MODE,
    // CHOOSE_SINGLE_MAP,
    PLAY_SINGLE_GAME,
    PLAY_DUAL_GAME,
    CHOOSE_ROOM,
    IN_ROOM,
    WAITING_OTHER,
};

enum AppState state = MAIN_MENU; // Start in the main menu state
