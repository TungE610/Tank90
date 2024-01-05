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
#include "constants.h"
#include "objects.h"
#include "client_udp.h"
#include "constants.h"
#include "messages.h"
#include "maps.h"

#define SERV_IP "127.0.0.1"
#define SERV_PORT 5500
#define NUM_ROWS 4
#define NUM_COLS 4
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 540;
int tankX = 220;
int tankY = 245;
int tankWidth = 30;
int tankHeight = 30;
int myId = 0;
int in_room_id = 0;
int foundRoomState = 1;
int room_nums = 0;
int ready_to_start = 0;

SDL_Texture *tank = NULL;
SDL_Texture *block = NULL;
SDL_Texture *iron = NULL;
SDL_Texture *grass = NULL;
SDL_Texture *water = NULL;
SDL_Texture *meUp = NULL;
SDL_Texture *meDown = NULL;
SDL_Texture *meLeft = NULL;
SDL_Texture *meRight = NULL;
SDL_Texture *enermy_1_right = NULL;
SDL_Texture *enermy_1_left = NULL;
SDL_Texture *enermy_1_up = NULL;
SDL_Texture *enermy_1_down = NULL;
SDL_Texture *none = NULL;

int positive = 1;
SDL_Renderer *renderer;
char *SERVER_ADDR = "127.0.0.1";

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

Room rooms[50];

enum AppState {
    MAIN_MENU,
    LOGIN,
    REGISTER,
    CHOOSE_MODE,
    PLAY_SINGLE_GAME,
    PLAY_DUAL_GAME,
    CHOOSE_ROOM,
    IN_ROOM,
    WAITING_OTHER,
};

#define totalBullets 20
SDL_Rect controlRect = { 240, 440, 40, 40 };
SDL_Rect friendRect = { 400, 440, 40, 40 };
int first_user = 0;

void setFocusOnTextbox(Textbox *textbox, int mouseX, int mouseY);
void moveDown(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect);
void moveUp(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect);
void moveRight(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect);
void moveLeft(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect);
void extractAndChangeValues(const char *input, int *id, int *state, int *first_player_id, int *second_player_id);
void renderRooms(SDL_Renderer *renderer, SDL_Texture *roomTexture, TTF_Font *font);
void checkRooms();
void* receiveThread(void* arg);
void renderDualModeGame(SDL_Renderer *renderer, SDL_Texture *myTank, SDL_Texture *friendTank, SDL_Texture* bulletTexture[]);

Bullet bullet[totalBullets];
SDL_Rect bulletRect[totalBullets];

void initRoom(Room *room, int id, int status, int first_player_id, int second_player_id) {
    room->id = id;
    room->status = status;
    room->first_player_id = first_player_id;
    room->second_player_id = second_player_id;
}

void initTextbox(Textbox *textbox, int x, int y, int w, int h) {
    strcpy(textbox->text, " \0");
    textbox->cursorPosition = 0;
    textbox->boxRect = (SDL_Rect){x, y, w, h};
    textbox->hasFocus = false;
}

void updatePlayerPosition(int *y, int *x, int deltaY, int deltaX) {
    dual_map_1[*y][*x] = 0;
    *y += deltaY;
    *x += deltaX;

    if (first_user == 1 && positive == 1) {
        dual_map_1[*y][*x] = 9;
    } else if (first_user == 0 && positive == 1) {
        dual_map_1[*y][*x] = 8;
    } else if (first_user == 1 && positive == 0) {
        dual_map_1[*y][*x] = 8;
    } else {
        dual_map_1[*y][*x] = 9;
    }
}

void initButton(Button *button, int x, int y, int w, int h, const char *text) {
    strcpy(button->text, text);
    button->boxRect = (SDL_Rect){x, y, w, h};
    button->hasFocus = false;
}

void handleKeyboardEvent(SDL_Event *e, Textbox *textbox) {
    if (textbox->hasFocus){
        if (e->type == SDL_KEYDOWN) {
            if (e->key.keysym.sym == SDLK_BACKSPACE && strlen(textbox->text) > 1) {
                textbox->text[strlen(textbox->text) - 1] = '\0';
            } else if (e->key.keysym.sym == SDLK_RETURN) {

            }
        } else if (e->type == SDL_TEXTINPUT) {
            strcat(textbox->text, e->text.text);
        }
    }

}

int isValidPosition(int newX, int newY) {
    // Check if the new position is within the map boundaries
    if (newX >= 0 && newX < 12 && newY >= 0 && newY < 12) {
        // Check if the new position is not a brick or iron
        if (single_map_1[newY][newX] != 1 && single_map_1[newY][newX] != 2) {
            return 1; // The position is valid (true)
        }
    }
    return 0; // The position is not valid (false)
}
enum AppState state = MAIN_MENU; // Start in the main menu state
int dual_mode_position_x, dual_mode_position_y;
int dual_mode_position_x_friend, dual_mode_position_y_friend;
SDL_Texture *friendTank;

int vertical_controller, hozirontal_controller;
int vertical_controller_friend, hozirontal_controller_friend;

int main(){

    Uint32 prevFrameTime = SDL_GetTicks();

	int client_sock;
	unsigned char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent,bytes_received, sin_size;
	SDL_Texture *texture = NULL;
	SDL_Texture *loading = NULL;
	SDL_Texture *playing = NULL;

    int single_mode_postion_x;
    int single_mode_postion_y;


    int menuChoose = 1;
    int chooseMode = 1;
    int single_mode_stage = 1;
    char login_message[BUFF_SIZE];
    char register_message[BUFF_SIZE];

    strcpy(login_message, " ");
    strcpy(register_message, " ");
    int mouseX, mouseY;
    pthread_t tid;
    int receiveThreadCreated = 0;
    
    TTF_Init();
    sin_size = sizeof(server_addr);
    int player_id;

    // Step 1: Construct a UDP socket
    if ((client_sock = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Define the address of the server
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERV_IP);

    // Step 3: nonnec to server
    if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

    SDL_Window* window = NULL;
    
    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );

    }else{
        //Create window
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }  else {
            renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            if (renderer == NULL) {
                SDL_DestroyWindow(window);
                printf("Could not create renderer: %s\n", SDL_GetError());
                SDL_Quit();
                return 1;
            }
            //Get window surface
            screenSurface = SDL_GetWindowSurface( window );

            //Fill the surface white
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0xFF, 0xFF ) );
            
            //Update the surface
            SDL_UpdateWindowSurface( window );

            //Hack to get window to stay up
            SDL_Event e; bool quit = false;
            IMG_Init(IMG_INIT_PNG);
            // load first pic
            TTF_Font *welcomeFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 50);  // Adjust font size as needed
            if (welcomeFont == NULL) {
                printf("Error loading font: %s\n", TTF_GetError());
                return 1;
            }

            TTF_Font *loginFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);  // Adjust font size as needed
            if (loginFont == NULL) {
                printf("Error loading font: %s\n", TTF_GetError());
                return 1;
            }

            // Create a text surface
            SDL_Color white = {255, 255, 255};  // White text color
            SDL_Color red = {255, 0, 0};  // White text color
            SDL_Surface *textSurface = TTF_RenderText_Solid(welcomeFont, "TANK 90", white);

            SDL_Surface *loginText = TTF_RenderText_Solid(loginFont, "1. Login", white);
            SDL_Surface *registerText = TTF_RenderText_Solid(loginFont, "2. Register", white);

            // Create a texture from the text surface
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);      
            SDL_Texture *loginTextTexture = SDL_CreateTextureFromSurface(renderer, loginText);      
            SDL_Texture *registerTextTexture = SDL_CreateTextureFromSurface(renderer, registerText);
            
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);  // Black background
            SDL_RenderClear(renderer);

            // Render the text texture
            SDL_Rect renderQuad = { 150, 150, textSurface->w, textSurface->h };
            SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);

            // render login text texture
            SDL_Rect loginTextRenderQuad = { 220, 230, loginText->w, loginText->h };
            SDL_RenderCopy(renderer, loginTextTexture, NULL, &loginTextRenderQuad);

            // render login text texture
            SDL_Rect registerTextRenderQuad = { 220, 280, registerText->w, registerText->h };
            SDL_RenderCopy(renderer, registerTextTexture, NULL, &registerTextRenderQuad);

            // load tank icon to choosee
            tank = IMG_LoadTexture(renderer, "images/player_green_up.png");
            block = IMG_LoadTexture(renderer, "images/block.png");
            iron = IMG_LoadTexture(renderer, "images/iron.png");
            grass = IMG_LoadTexture(renderer, "images/grass.png");
            water = IMG_LoadTexture(renderer, "images/water.png");
            meUp = IMG_LoadTexture(renderer, "images/me_up.png");
            meRight = IMG_LoadTexture(renderer, "images/me_right.png");
            meLeft = IMG_LoadTexture(renderer, "images/me_left.png");
            meDown = IMG_LoadTexture(renderer, "images/me_down.png");
            enermy_1_right = IMG_LoadTexture(renderer, "images/enermy_right.png");
            enermy_1_left = IMG_LoadTexture(renderer, "images/enermy_left.png");
            enermy_1_up = IMG_LoadTexture(renderer, "images/enermy_up.png");
            enermy_1_down = IMG_LoadTexture(renderer, "images/enermy_down.png");
            none = IMG_LoadTexture(renderer, "images/none.png");

            int tankwidth = 30;
            int tankheight = 30;
            SDL_Rect tankRect = {180, 225, tankwidth, tankheight};

            TTF_Font *loginBigFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 50);  // Adjust font size as needed
            TTF_Font *usernameFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);  // Adjust font size as needed
            TTF_Font *chooseModeFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 35);  // Adjust font size as needed

            if (loginFont == NULL || usernameFont == NULL) {
                printf("Error loading font: %s\n", TTF_GetError());
            }

            SDL_Surface *loginBigText = TTF_RenderText_Solid(loginBigFont, "LOGIN", white);
            SDL_Surface *registerBigText = TTF_RenderText_Solid(loginBigFont, "REGISTER", white);
            SDL_Surface *roomsBigText = TTF_RenderText_Solid(loginBigFont, "ROOMS", white);
            SDL_Surface *usernameText = TTF_RenderText_Solid(usernameFont, "Username: ", white);
            SDL_Surface *retypePasswordText = TTF_RenderText_Solid(usernameFont, "Retype Password: ", white);
            SDL_Surface *passwordText = TTF_RenderText_Solid(usernameFont, "Password: ", white);
            SDL_Surface *chooseModeText = TTF_RenderText_Solid(loginBigFont, "Choose Mode", white);
            SDL_Surface *singleModeText = TTF_RenderText_Solid(usernameFont, "1 Person", white);
            SDL_Surface *dualModeText = TTF_RenderText_Solid(usernameFont, "2 People", white);

            SDL_Texture *loginBigTextTexture = SDL_CreateTextureFromSurface(renderer, loginBigText); 
            SDL_Texture *registerBigTextTexture = SDL_CreateTextureFromSurface(renderer, registerBigText); 
            SDL_Texture *roomsBigTextTexture = SDL_CreateTextureFromSurface(renderer, roomsBigText);
            SDL_Texture *usernameTextTexture = SDL_CreateTextureFromSurface(renderer, usernameText); 
            SDL_Texture *passwordTextTexture = SDL_CreateTextureFromSurface(renderer, passwordText);
            SDL_Texture *retypePasswordTextTexture = SDL_CreateTextureFromSurface(renderer, retypePasswordText);
            SDL_Texture *chooseModeTextTexture = SDL_CreateTextureFromSurface(renderer, chooseModeText);  
            SDL_Texture *singleModeTextTexture = SDL_CreateTextureFromSurface(renderer, singleModeText);      
            SDL_Texture *dualModeTextTexture = SDL_CreateTextureFromSurface(renderer, dualModeText);      


            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);  // Black background
            SDL_RenderClear(renderer);

            // Render the text texture
            SDL_Rect loginBigTextRenderQuad = { 195, 100, loginBigText->w, loginBigText->h };
            SDL_Rect roomsBigTextRenderQuad = { 195, 100, roomsBigText->w, roomsBigText->h };
            SDL_Rect usernameRenderQuad = { 120, 200, usernameText->w, usernameText->h };
            SDL_Rect passwordRenderQuad = { 120, 250, passwordText->w, passwordText->h };
            SDL_Rect retypePasswordRenderQuad = { 120, 300, retypePasswordText->w, retypePasswordText->h };
            SDL_Rect chooseModeRenderQuad = { 50, 100, chooseModeText->w, chooseModeText->h };
            SDL_Rect singleModeRenderQuad = { 250, 250, singleModeText->w, singleModeText->h };
            SDL_Rect dualModeRenderQuad = { 250, 300, dualModeText->w, dualModeText->h };
            SDL_Rect tankRectChooseMode = {200, 242, tankwidth, tankheight};

            Textbox usernameLoginTextbox;
            Textbox passwordLoginTextbox;
            initTextbox(&usernameLoginTextbox, 320, 195, 200, 30);
            initTextbox(&passwordLoginTextbox, 320, 245, 200, 30);

            Textbox usernameRegisterTextbox;
            Textbox passwordRegisterTextbox;
            Textbox retypePasswordRegisterTextbox;
            
            initTextbox(&usernameRegisterTextbox, 300, 195, 200, 30);
            initTextbox(&passwordRegisterTextbox, 300, 245, 200, 30);
            initTextbox(&retypePasswordRegisterTextbox, 410, 295, 200, 30);

            SDL_Texture *myTank = meUp;
            friendTank = meUp;

            SDL_Color textColor = {255, 255, 255, 255}; // White text
            createSingleMap1(single_map_1);
            createDualMap1(dual_map_1);

            TTF_Font *loginMessageFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 13);  // Adjust font size as needed

            // Initialize bullets and rects
            for (int i = 0; i < totalBullets; i++) {
                bullet[i].position_x = hozirontal_controller;
                bullet[i].position_y = vertical_controller;
                bullet[i].is_active = 0;  // Assuming first bullet is active
                bulletRect[i] = (SDL_Rect){ controlRect.x, controlRect.y, 30, 30 };
            }

            Enermy enermy[5];
            SDL_Rect enermyRect[5];

            for (int i = 0; i < 5; i ++) {
                enermy[i].position_x = 11;
                enermy[i].position_y = 0;
                enermy[i].direction = LEFT;
                enermy[i].blood = 1;
                enermyRect[i] = (SDL_Rect){ 440, 0, 40, 40 };
                enermy[i].movementTimer = 0.0f; // Initialize movement timer to zero
            }

            while( quit == false ){
                printf("check control x: %d\n", controlRect.x);
                printf("check control y: %d\n", controlRect.y);
                printf("check friend x: %d\n", friendRect.x);
                printf("check friend y: %d\n", friendRect.y);

                SDL_Texture *bulletTexture[totalBullets];
                for (int i = 0; i < totalBullets; i ++) {
                    if (bullet[i].direction == UP) {
                        bulletTexture[i] = IMG_LoadTexture(renderer, "./images/bullet_up.png");
                    } else if (bullet[i].direction == DOWN) {
                        bulletTexture[i] = IMG_LoadTexture(renderer, "./images/bullet_down.png");
                    } else if (bullet[i].direction == RIGHT) {
                        bulletTexture[i] = IMG_LoadTexture(renderer, "./images/bullet_right.png");
                    } else {
                        bulletTexture[i] = IMG_LoadTexture(renderer, "./images/bullet_left.png");
                    }
                    
                    if (bullet[i].is_active == 0) {
                        bulletRect[i].x  = controlRect.x;
                        bulletRect[i].y  = controlRect.y;
                    }
                }

                SDL_Texture *enermyTexture[5];

                for (int i = 0; i < 5; i ++) {
                    if (enermy[i].direction == UP) {
                        enermyTexture[i] = IMG_LoadTexture(renderer, "./images/enermy_up.png");
                    } else if (enermy[i].direction == DOWN) {
                        enermyTexture[i] = IMG_LoadTexture(renderer, "./images/enermy_down.png");
                    } else if (enermy[i].direction == RIGHT) {
                        enermyTexture[i] = IMG_LoadTexture(renderer, "./images/enermy_right.png");
                    } else {
                        enermyTexture[i] = IMG_LoadTexture(renderer, "./images/enermy_left.png");
                    }
                }

                SDL_RenderClear(renderer);
                if (state == MAIN_MENU) {
                    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
                    SDL_RenderCopy(renderer, loginTextTexture, NULL, &loginTextRenderQuad);
                    SDL_RenderCopy(renderer, registerTextTexture, NULL, &registerTextRenderQuad);
                    SDL_RenderCopy(renderer, tank, NULL, &tankRect);
//------------------------------------------------------------------------------------------------------------------------------------------
                } else if (state == LOGIN) {
                    SDL_RenderCopy(renderer, loginBigTextTexture, NULL, &loginBigTextRenderQuad);
                    SDL_RenderCopy(renderer, usernameTextTexture, NULL, &usernameRenderQuad);
                    SDL_RenderCopy(renderer, passwordTextTexture, NULL, &passwordRenderQuad);
                    
                    SDL_Surface *loginMessgageText = TTF_RenderText_Solid(loginMessageFont, login_message, red);
                    SDL_Texture *loginMessageTextTexture = SDL_CreateTextureFromSurface(renderer, loginMessgageText);   
                    SDL_Rect loginMessageRenderQuad = { 120, 285, loginMessgageText->w, loginMessgageText->h }; 
                    SDL_RenderCopy(renderer, loginMessageTextTexture, NULL, &loginMessageRenderQuad);
                    SDL_FreeSurface(loginMessgageText); // Free the surface to avoid memory leak
                    SDL_DestroyTexture(loginMessageTextTexture); // Free the surface to avoid memory leak

                    // username box
                    SDL_Surface *usernameBoxTextSurface = TTF_RenderText_Solid(usernameFont, usernameLoginTextbox.text, textColor);
                    if (usernameBoxTextSurface == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error rendering text: %s\n", TTF_GetError());
                    }

                    // Create a texture from the surface
                    SDL_Texture *usernameBoxTextTexture = SDL_CreateTextureFromSurface(renderer, usernameBoxTextSurface);
                    if (usernameBoxTextTexture == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error creating texture: %s\n", SDL_GetError());
                        SDL_FreeSurface(usernameBoxTextSurface); // Free the surface to avoid memory leak
                    }

                    // Set rendering space and render to screen
                    int usernameTextWidth = usernameBoxTextSurface->w;
                    int usernameTextHeight = usernameBoxTextSurface->h;
                    SDL_Rect usernameBoxRenderQuad = { usernameLoginTextbox.boxRect.x + 5, usernameLoginTextbox.boxRect.y + 5, usernameTextWidth, usernameTextHeight };

                    //password box
                    SDL_Surface *passwordBoxTextSurface = TTF_RenderText_Solid(usernameFont, passwordLoginTextbox.text, textColor);
                    if (passwordBoxTextSurface == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error rendering text: %s\n", TTF_GetError());
                    }

                    // Create a texture from the surface
                    SDL_Texture *passwordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, passwordBoxTextSurface);
                    if (passwordBoxTextTexture == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error creating texture: %s\n", SDL_GetError());
                        SDL_FreeSurface(passwordBoxTextSurface); // Free the surface to avoid memory leak
                    }

                    // Set rendering space and render to screen
                    int passwordTextWidth = passwordBoxTextSurface->w;
                    int passwordTextHeight = passwordBoxTextSurface->h;
                    SDL_Rect passwordBoxRenderQuad = { passwordLoginTextbox.boxRect.x + 5, passwordLoginTextbox.boxRect.y + 5, passwordTextWidth, passwordTextHeight };

                    SDL_RenderCopy(renderer, usernameBoxTextTexture, NULL, &usernameBoxRenderQuad);
                    SDL_RenderCopy(renderer, passwordBoxTextTexture, NULL, &passwordBoxRenderQuad);

                    // render login button ---------------------------------------------------------------------------
                    Button loginButton;
                    initButton(&loginButton, 230, 330, 200, 40, "Login");

                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 20); // Blue color
                    SDL_Rect loginButtonRect = { loginButton.boxRect.x, loginButton.boxRect.y, loginButton.boxRect.w, loginButton.boxRect.h };
                    SDL_RenderFillRect(renderer, &loginButtonRect);

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25); // Black background
                    // Render the text on the button
                    SDL_Color buttonTextColor = {255, 255, 255, 255}; // White text
                    SDL_Surface *buttonTextSurface = TTF_RenderText_Solid(loginFont, loginButton.text, buttonTextColor);
                    SDL_Texture *buttonTextTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);
                    SDL_Rect buttonTextRenderQuad = { loginButton.boxRect.x + 50, loginButton.boxRect.y + 12, buttonTextSurface->w, buttonTextSurface->h };
                    SDL_RenderCopy(renderer, buttonTextTexture, NULL, &buttonTextRenderQuad);

//-----------------------------------------------------------------------------------------------------------------------------------------------
                } else if (state == REGISTER) {
                    SDL_RenderCopy(renderer, registerBigTextTexture, NULL, &loginBigTextRenderQuad);
                    SDL_RenderCopy(renderer, usernameTextTexture, NULL, &usernameRenderQuad);
                    SDL_RenderCopy(renderer, passwordTextTexture, NULL, &passwordRenderQuad);
                    SDL_RenderCopy(renderer, retypePasswordTextTexture, NULL, &retypePasswordRenderQuad);

                    SDL_Surface *registerMessgageText = TTF_RenderText_Solid(loginMessageFont, login_message, red);
                    SDL_Texture *registerMessageTextTexture = SDL_CreateTextureFromSurface(renderer, registerMessgageText);   
                    SDL_Rect registerMessageRenderQuad = { 120, 335, registerMessgageText->w, registerMessgageText->h }; 
                    SDL_RenderCopy(renderer, registerMessageTextTexture, NULL, &registerMessageRenderQuad);
                    SDL_FreeSurface(registerMessgageText); // Free the surface to avoid memory leak
                    SDL_DestroyTexture(registerMessageTextTexture); // Free the surface to avoid memory leak

                    // username box
                    SDL_Surface *usernameBoxTextSurface = TTF_RenderText_Solid(usernameFont, usernameRegisterTextbox.text, textColor);
                    if (usernameBoxTextSurface == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error rendering text: %s\n", TTF_GetError());
                    }

                    // Create a texture from the surface
                    SDL_Texture *usernameBoxTextTexture = SDL_CreateTextureFromSurface(renderer, usernameBoxTextSurface);
                    if (usernameBoxTextTexture == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error creating texture: %s\n", SDL_GetError());
                        SDL_FreeSurface(usernameBoxTextSurface); // Free the surface to avoid memory leak
                    }

                    // Set rendering space and render to screen
                    int usernameTextWidth = usernameBoxTextSurface->w;
                    int usernameTextHeight = usernameBoxTextSurface->h;
                    SDL_Rect usernameBoxRenderQuad = { usernameRegisterTextbox.boxRect.x + 5, usernameRegisterTextbox.boxRect.y + 5, usernameTextWidth, usernameTextHeight };

                    //password box
                    SDL_Surface *passwordBoxTextSurface = TTF_RenderText_Solid(usernameFont, passwordRegisterTextbox.text, textColor);
                    if (passwordBoxTextSurface == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error rendering text: %s\n", TTF_GetError());
                    }

                    // Create a texture from the surface
                    SDL_Texture *passwordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, passwordBoxTextSurface);
                    if (passwordBoxTextTexture == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error creating texture: %s\n", SDL_GetError());
                        SDL_FreeSurface(passwordBoxTextSurface); // Free the surface to avoid memory leak
                    }

                    // Set rendering space and render to screen
                    int passwordTextWidth = passwordBoxTextSurface->w;
                    int passwordTextHeight = passwordBoxTextSurface->h;
                    SDL_Rect passwordBoxRenderQuad = { passwordRegisterTextbox.boxRect.x + 5, passwordRegisterTextbox.boxRect.y + 5, passwordTextWidth, passwordTextHeight };

                     //password box
                    SDL_Surface *retypePasswordRegisterBoxTextSurface = TTF_RenderText_Solid(usernameFont, retypePasswordRegisterTextbox.text, textColor);
                    if (retypePasswordRegisterBoxTextSurface == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error rendering text: %s\n", TTF_GetError());
                    }

                    // Create a texture from the surface
                    SDL_Texture *retypePasswordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, retypePasswordRegisterBoxTextSurface);
                    if (retypePasswordBoxTextTexture == NULL) {
                        // Handle the error, e.g., log it and return
                        printf("Error creating texture: %s\n", SDL_GetError());
                        SDL_FreeSurface(retypePasswordRegisterBoxTextSurface); // Free the surface to avoid memory leak
                    }

                    // Set rendering space and render to screen
                    int retypePasswordTextWidth = retypePasswordRegisterBoxTextSurface->w;
                    int retypePasswordTextHeight = retypePasswordRegisterBoxTextSurface->h;
                    SDL_Rect retypePasswordBoxRenderQuad = { retypePasswordRegisterTextbox.boxRect.x + 5, retypePasswordRegisterTextbox.boxRect.y + 5, retypePasswordTextWidth, retypePasswordTextHeight };

                    SDL_RenderCopy(renderer, usernameBoxTextTexture, NULL, &usernameBoxRenderQuad);
                    SDL_RenderCopy(renderer, passwordBoxTextTexture, NULL, &passwordBoxRenderQuad);
                    SDL_RenderCopy(renderer, retypePasswordBoxTextTexture, NULL, &retypePasswordBoxRenderQuad);

                    // render login button ---------------------------------------------------------------------------
                    Button regiterButton;
                    initButton(&regiterButton, 230, 380, 200, 40, "Register");

                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 20); // Blue color
                    SDL_Rect regiterButtonRect = { regiterButton.boxRect.x, regiterButton.boxRect.y, regiterButton.boxRect.w, regiterButton.boxRect.h };
                    SDL_RenderFillRect(renderer, &regiterButtonRect);

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25); // Black background
                    // Render the text on the button
                    SDL_Color buttonTextColor = {255, 255, 255, 255}; // White text
                    SDL_Surface *buttonTextSurface = TTF_RenderText_Solid(loginFont, regiterButton.text, buttonTextColor);
                    SDL_Texture *buttonTextTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);
                    SDL_Rect buttonTextRenderQuad = { regiterButton.boxRect.x + 20, regiterButton.boxRect.y + 12, buttonTextSurface->w, buttonTextSurface->h };
                    SDL_RenderCopy(renderer, buttonTextTexture, NULL, &buttonTextRenderQuad);

//-------------------------------------------------------------------------------------------------------------------------------------------
                } else if (state == CHOOSE_MODE) {
                    SDL_RenderCopy(renderer, chooseModeTextTexture, NULL, &chooseModeRenderQuad);
                    SDL_RenderCopy(renderer, singleModeTextTexture, NULL, &singleModeRenderQuad);
                    SDL_RenderCopy(renderer, dualModeTextTexture, NULL, &dualModeRenderQuad);
                    SDL_RenderCopy(renderer, tank, NULL, &tankRectChooseMode);
                } else if (state == PLAY_SINGLE_GAME && single_mode_stage == 1) {

                            // Set the background color to black
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderClear(renderer);

                // Set the draw color for the gray border
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                // Calculate the map boundaries
                int mapX = (640 - (12 * 40)) / 2;
                int mapY = (480 - (12 * 40)) / 2;

                // Render the gray border around the map
                SDL_Rect borderRect = { mapX - 2, mapY - 2, 12 * 40 + 4, 12 * 40 + 4 };
                SDL_RenderFillRect(renderer, &borderRect);

                // Render the map
                for (int y = 0; y < 12; y++) {
                    for (int x = 0; x < 12; x++) {
                        SDL_Rect destRect = { mapX + x * 40, mapY + y * 40, 40, 40 };
                        SDL_Texture* texture = NULL;

                        switch (single_map_1[y][x]) {
                            case 0:
                                // texture= none;
                                break;
                            case 1:
                                texture = block;
                                break;
                            case 2:
                                texture = iron;
                                break;
                            case 9:
                                texture = myTank;
                                break;
                            case 7:
                                texture = enermy_1_right;
                                break;
                            default:
                                break;
                        }

                        if (texture == meUp || texture == meDown || texture == meRight || texture == meLeft) {
                            SDL_RenderCopy(renderer, texture, NULL, &controlRect);
                        } else {
                            SDL_RenderCopy(renderer, texture, NULL, &destRect);
                        }
                    }
                }
                        int previousDirection[5] = {-1, -1, -1, -1, -1}; // Initialize with invalid values

//-------------------Render enermy----------------------------------------------------------------------------------------------
                        for (int i = 0; i < 5; i++) {
                            int direction;
                            // Determine the direction for the enemy's movement (0: up, 1: down, 2: left, 3: right)
                            do {
                                direction = rand() % 4;
                            } while (direction == previousDirection[i]);
                            previousDirection[i] = direction;
                            // Update the enermyRect based on the chosen direction
                            switch (direction) {
                                case 0: // Move up               
                                    if (enermyRect[i].y % 8 == 0 && enermy[i].position_y -1 >= 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0) {
                                        if (enermyRect[i].x % 8 != 0 && enermyRect[i].x < enermy[i].position_x * 8) {
                                            if (single_map_1[enermy[i].position_y - 1][enermy[i].position_x - 1] == 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0) {
                                                enermyRect[i].y -= 5;
                                            }
                                        } else if (enermyRect[i].x % 8 != 0 && enermyRect[i].x > enermy[i].position_x * 8) {
                                            if (single_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x + 1] == 0) {
                                                enermyRect[i].y -= 5;
                                            }
                                        } else if (enermyRect[i].x % 8 == 0) {
                                            enermyRect[i].y -= 5;
                                        }
                                    } else if (enermyRect[i].y % 8 == 7 && enermyRect[i].y + 1 == (enermy[i].position_y + 1) * 8) {
                                        enermyRect[i].y -= 5;
                                        enermy[i].position_y--;
                                    } else if (enermyRect[i].y % 8 != 0) {
                                        enermyRect[i].y -= 5;
                                    }
                                    break;
                               case 1: // Move down
                                    if (enermyRect[i].y % 8 == 0 && enermy[i].position_y + 1< 12 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0) {
                                        if (enermyRect[i].x % 8 != 0 && enermyRect[i].x < enermy[i].position_x * 8) {
                                            if (single_map_1[enermy[i].position_y + 1][enermy[i].position_x - 1] == 0 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0) {
                                                enermyRect[i].y+=5;
                                            }
                                        } else if (enermyRect[i].x % 8 != 0 && enermyRect[i].x > enermy[i].position_x * 8) {
                                            if (single_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x + 1] == 0) {
                                                enermyRect[i].y+=5;
                                            }
                                        } else if (enermyRect[i].x % 8 == 0) {
                                            enermyRect[i].y+=5;
                                        }
                                    } else if (enermyRect[i].y % 8 == 1 && enermyRect[i].y - 1 == (enermy[i].position_y - 1) * 8) {
                                        enermyRect[i].y+=5;
                                        enermy[i].position_y++;
                                    } else if (enermyRect[i].y % 8 != 0) {
                                        enermyRect[i].y+=5;
                                    }
                                break;
                                case 2: // Move left
                                    if (enermyRect[i].x % 8 == 0 && enermy[i].position_x - 1 >= 0 && single_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0) {
                                        if (enermyRect[i].y % 8 != 0 && enermyRect[i].y < enermy[i].position_y * 8) {
                                            if (single_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x - 1] == 0) {
                                                enermyRect[i].x-=5;
                                            }
                                        } else if (enermyRect[i].y % 8 != 0 && enermyRect[i].y > enermy[i].position_y * 8) {
                                            if (single_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x - 1] == 0) {
                                                enermyRect[i].x-=5;
                                            }
                                        } else if (enermyRect[i].y % 8 == 0) {
                                            enermyRect[i].x-=5;
                                        }
                                    } else if (enermyRect[i].x % 8 == 1 && enermyRect[i].x - 1 == (enermy[i].position_x - 1) * 8) {
                                        enermyRect[i].x-=5;
                                        enermy[i].position_x--;
                                    } else if (enermyRect[i].x % 8 != 0) {
                                        enermyRect[i].x-=5;
                                    }
                                    break;
                               case 3: // Move right
                                if (enermyRect[i].x % 8 == 0 && enermy[i].position_x + 1 < 12 && single_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0) {
                                    if (enermyRect[i].y % 8 != 0 && enermyRect[i].y < enermy[i].position_y * 8) {
                                        if (single_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x + 1] == 0) {
                                            enermyRect[i].x+=5;
                                        }
                                    } else if (enermyRect[i].y % 8 != 0 && enermyRect[i].y > enermy[i].position_y * 8) {
                                        if (single_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x + 1] == 0) {
                                            enermyRect[i].x+=5;
                                        }
                                    } else if (enermyRect[i].y % 8 == 0) {
                                        enermyRect[i].x+=5;
                                    }
                                } else if (enermyRect[i].x % 8 == 7 && enermyRect[i].x + 1 == (enermy[i].position_x + 1) * 8) {
                                    enermyRect[i].x+=5;
                                    enermy[i].position_x++;
                                } else if (enermyRect[i].x % 8 != 7) {
                                    enermyRect[i].x+=5;
                                }
                                break;
                                default:
                                    break;
                            }

                        }
                        for (int i = 0; i < 5; i++) {
                            SDL_RenderCopy(renderer, enermyTexture[i], NULL, &enermyRect[i]);
                        }

///---------------------end render enermy-----------------------------------------------------------
                for (int i = 0; i < totalBullets; i ++) {
                    if (bullet[i].is_active == 1) {
                        SDL_RenderCopy(renderer, bulletTexture[i], NULL, &bulletRect[i]);
                    }
                    if (bullet[i].is_active == 1) {
                        if (bullet[i].direction == UP) {
                            // bullet[i].position_y = bulletRect[i].y / 5;
                            if (single_map_1[(bulletRect[i].y)/40 - 1][(bulletRect[i].x-80)/40] != 0 && bulletRect[i].y % 40 == 0) {
                                bullet[i].is_active = 0;
                                SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
                            } else {
                                bulletRect[i].y -= 1;
                            }
                        } else if (bullet[i].direction == DOWN) {
                            if (single_map_1[(bulletRect[i].y)/40 + 1][(bulletRect[i].x-80)/40] != 0 && bulletRect[i].y % 40 == 0) {
                                bullet[i].is_active = 0;
                                SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
                            }
                            bulletRect[i].y += 1;
                            // bullet[i].position_y = bulletRect[i].y / 5;
                        } else if (bullet[i].direction == RIGHT) {
                            if (single_map_1[(bulletRect[i].y)/40][(bulletRect[i].x-80)/40 + 1] != 0 && bulletRect[i].x % 40 == 0) {
                                bullet[i].is_active = 0;
                                SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
                            }
                            bulletRect[i].x += 1;
                            // bullet[i].position_x = bulletRect[i].x / 5;
                        } else {
                            if (single_map_1[(bulletRect[i].y)/40][(bulletRect[i].x-80)/40 - 1] != 0 && bulletRect[i].x % 40 == 0) {
                                bullet[i].is_active = 0;
                                SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
                            }
                            bulletRect[i].x -= 1;
                            // bullet[i].position_x = bulletRect[i].x / 5;
                        }
                    }
                }
                } else if (state == CHOOSE_ROOM) {
                    SDL_RenderClear(renderer);

                // Set the draw color for the gray border
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderCopy(renderer, roomsBigTextTexture, NULL, &roomsBigTextRenderQuad);

                    // render create room button
                    Button createRoomButton;
                    initButton(&createRoomButton, 480, 110, 80, 40, " + ");

                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 20); // Blue color
                    SDL_Rect createRoomButtonRect = { createRoomButton.boxRect.x, createRoomButton.boxRect.y, createRoomButton.boxRect.w, createRoomButton.boxRect.h };
                    SDL_RenderFillRect(renderer, &createRoomButtonRect);

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25); // Black background
                    // Render the text on the button
                    SDL_Color buttonTextColor = {255, 255, 255, 255}; // White text
                    TTF_Font *createRoomFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);  // Adjust font size as needed
                    SDL_Surface *buttonTextSurface = TTF_RenderText_Solid(createRoomFont, createRoomButton.text, buttonTextColor);
                    SDL_Texture *buttonTextTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);
                    SDL_Rect buttonTextRenderQuad = { createRoomButton.boxRect.x + 15, createRoomButton.boxRect.y + 15, buttonTextSurface->w, buttonTextSurface->h };
                    SDL_RenderCopy(renderer, buttonTextTexture, NULL, &createRoomButtonRect);

                    if (foundRoomState == 0) {
                        SDL_Texture *notfoundTexture = IMG_LoadTexture(renderer, "images/nothingfound.png");
                        SDL_Rect notfoundRenderQuad = {280, 220, 50, 50};

                        TTF_Font *notfoundFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);  // Adjust font size as needed
                        SDL_Surface *notfoundTextSurface = TTF_RenderText_Solid(notfoundFont, "no room found...", white);
                        SDL_Texture *notfoundTextTexture = SDL_CreateTextureFromSurface(renderer, notfoundTextSurface);
                        SDL_Rect notfoundTextRenderQuad = { 190, 300, notfoundTextSurface->w, notfoundTextSurface->h };
                        SDL_RenderCopy(renderer, notfoundTexture, NULL, &notfoundRenderQuad);
                        SDL_RenderCopy(renderer, notfoundTextTexture, NULL, &notfoundTextRenderQuad);
                    } else {
                    //     for (int i = 1; i <= room_nums; i ++) {
                            SDL_Texture *roomTexture = IMG_LoadTexture(renderer, "images/room.png");
                            TTF_Font *font = TTF_OpenFont("./resources/font/ARCADE.TTF", 30);
                            renderRooms(renderer, roomTexture, font);
                    //     }
                    }

                } else if (state == PLAY_DUAL_GAME) {
                    renderDualModeGame(renderer, myTank, friendTank, bulletTexture);

                } else if (state == WAITING_OTHER) {
                    SDL_RenderClear(renderer);

                // Set the draw color for the gray border
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                    char in_room_sid[2];
                    sprintf(in_room_sid, "%d", in_room_id);

                    char title[BUFF_SIZE];
                    strcpy(title, "ROOM ID:"); // Use strcpy to initialize the buffer
                    strcat(title, in_room_sid);

                    SDL_Surface *roomIdBigText = TTF_RenderText_Solid(loginBigFont, title, white);
                    SDL_Texture *roomIdBigTextTexture = SDL_CreateTextureFromSurface(renderer, roomIdBigText);
                    SDL_Rect roomIdBigTextRenderQuad = { 110, 100, roomIdBigText->w, roomIdBigText->h };

                    SDL_RenderCopy(renderer, roomIdBigTextTexture, NULL, &roomIdBigTextRenderQuad);

                    SDL_Texture *waitingTank = IMG_LoadTexture(renderer, "./images/waiting.png");
                    SDL_Rect waitingTankRect = {245, 250, 140, 100};

                    TTF_Font *waitingRoomFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);  // Adjust font size as needed
                    SDL_Surface *waitingRoomTextSurface = TTF_RenderText_Solid(waitingRoomFont, "waiting for another...", white);
                    SDL_Texture *waitingRoomTextTexture = SDL_CreateTextureFromSurface(renderer, waitingRoomTextSurface);
                    SDL_Rect waitingTextRenderQuad = { 120, 380, waitingRoomTextSurface->w, waitingRoomTextSurface->h };
                // // Set the draw color for the gray border
                    SDL_RenderCopy(renderer, waitingRoomTextTexture, NULL, &waitingTextRenderQuad);
                    SDL_RenderCopy(renderer, waitingTank, NULL, &waitingTankRect);
                } else if (state == IN_ROOM){
                    SDL_RenderClear(renderer);

                // Set the draw color for the gray border
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                    char in_room_sid[2];
                    sprintf(in_room_sid, "%d", in_room_id);

                    char title[BUFF_SIZE];
                    strcpy(title, "ROOM ID:"); // Use strcpy to initialize the buffer
                    strcat(title, in_room_sid);

                    SDL_Surface *roomIdBigText = TTF_RenderText_Solid(loginBigFont, title, white);
                    SDL_Texture *roomIdBigTextTexture = SDL_CreateTextureFromSurface(renderer, roomIdBigText);
                    SDL_Rect roomIdBigTextRenderQuad = { 110, 100, roomIdBigText->w, roomIdBigText->h };

                    SDL_RenderCopy(renderer, roomIdBigTextTexture, NULL, &roomIdBigTextRenderQuad);

                
                    SDL_Texture *readyTank = IMG_LoadTexture(renderer, "./images/readyTank.png");
                    SDL_Rect readyTankRect = {245, 250, 140, 100};
                    
                    SDL_RenderCopy(renderer, readyTank, NULL, &readyTankRect);

                    Button startButton;
                    initButton(&startButton, 220, 370, 200, 40, "Start");

                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 20); // Blue color
                    SDL_Rect startButtonRect = { startButton.boxRect.x, startButton.boxRect.y, startButton.boxRect.w, startButton.boxRect.h };
                    SDL_RenderFillRect(renderer, &startButtonRect);

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25); // Black background
                    // Render the text on the button
                    SDL_Color startButtonTextColor = {255, 255, 255, 255}; // White text
                    SDL_Surface *startButtonTextSurface = TTF_RenderText_Solid(loginFont, startButton.text, startButtonTextColor);
                    SDL_Texture *startButtonTextTexture = SDL_CreateTextureFromSurface(renderer, startButtonTextSurface);
                    SDL_Rect startButtonTextRenderQuad = { startButton.boxRect.x + 50, startButton.boxRect.y + 12, startButtonTextSurface->w, startButtonTextSurface->h };
                    SDL_RenderCopy(renderer, startButtonTextTexture, NULL, &startButtonTextRenderQuad);
                }

                SDL_RenderPresent(renderer);
                
                while( SDL_PollEvent( &e ) ){ 
                    if (e.type == SDL_QUIT) {
                        quit = true;

                    } else if (e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT) {
                        switch (state) {
                            case MAIN_MENU:
                                switch (e.key.keysym.sym) {
                                    case SDLK_DOWN:
                                        if (tankRect.y < 275) {

                                            tankRect.y += 50;
                                            menuChoose = 2;
                                        }
                                        break;
                                    case SDLK_UP:
                                        if (tankRect.y > 225) {

                                            tankRect.y -= 50;
                                            menuChoose = 1;
                                        }
                                        break;
                                    case SDLK_RETURN:
                                        if (menuChoose == 1) {
                                            state = LOGIN;

                                        } else if(menuChoose == 2){
                                            state = REGISTER;
                                        }

                                        break;
                                }
                            break;
                            case LOGIN:
                                    handleKeyboardEvent(&e, &usernameLoginTextbox);
                                    handleKeyboardEvent(&e, &passwordLoginTextbox);
                                break;
                            case REGISTER:
                                handleKeyboardEvent(&e, &usernameRegisterTextbox);
                                handleKeyboardEvent(&e, &passwordRegisterTextbox);
                                handleKeyboardEvent(&e, &retypePasswordRegisterTextbox);
                                break;
                            case CHOOSE_MODE:
                                switch (e.key.keysym.sym) {
                                        case SDLK_DOWN:
                                            if (tankRectChooseMode.y < 292) {

                                                tankRectChooseMode.y += 50;
                                                chooseMode = 2;
                                            }
                                            
                                            break;
                                        case SDLK_UP:
                                            if (tankRectChooseMode.y > 242) {

                                                tankRectChooseMode.y -= 50;
                                                chooseMode = 1;
                                            }
                                            break;
                                        case SDLK_RETURN:
                                            if (chooseMode == 1) {
                                                state = PLAY_SINGLE_GAME;
                                            } else {
                                                char chooseRoomMessage[BUFF_SIZE];
                                                strcpy(chooseRoomMessage, createChooseRoomMessage());

                                                bytes_sent = send(client_sock, chooseRoomMessage, strlen(chooseRoomMessage), 0);
                                                if (bytes_sent < 0) {
                                                    perror("Error sending message: ");
                                                }

                                                bytes_received =recv(client_sock, buff, sizeof(buff), 0);
                                                buff[bytes_received] = '\0';

                                                // buff[bytes_received] = '\0';
                                                int waitingRoomNum = atoi(buff);
                                                bytes_sent = send(client_sock, "ok", strlen("ok"), 0);

                                                if (waitingRoomNum == 0) {
                                                    foundRoomState = 0;
                                                } else {
                                                    for (int i = 1; i <= waitingRoomNum; i ++) {
                                                        bytes_received =recv(client_sock, buff, sizeof(buff), 0);
                                                        foundRoomState ++;
                                                        int id, state,  first_player_id, second_player_id;
                                                        extractAndChangeValues(buff, &id, &state, &first_player_id, &second_player_id);
                                                        room_nums ++;
                                                        initRoom(&rooms[room_nums], id, state, first_player_id, second_player_id);
                                                    }
                                                }
                                                state = CHOOSE_ROOM;
                                            }

                                            break;
                                }

                                break;
                            case PLAY_SINGLE_GAME:
                                switch (e.key.keysym.sym) {
                                        case SDLK_DOWN:
                                            moveDown(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &controlRect);
                                            myTank = meDown;
                                            break;
                                        case SDLK_UP:
                                            moveUp(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &controlRect);
                                            myTank = meUp;
                                            break;
                                        case SDLK_RIGHT:
                                            moveRight(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &controlRect);
                                            myTank = meRight;
                                            break;
                                        case SDLK_LEFT:
                                            moveLeft(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &controlRect);
                                            myTank = meLeft;
                                            break;
                                    case SDLK_RETURN:
                                        break;
                                    case SDLK_SPACE:
                                        //  shot(controlRect.x, controlRect.y, 1, renderer);
                                        for (int i =0; i < totalBullets; i ++) {
                                            if (bullet[i].is_active == 0) {
                                                bullet[i].is_active = 1;
                                                if (myTank == meUp) {
                                                     bullet[i].direction = UP;

                                                } else if (myTank == meDown) {
                                                     bullet[i].direction = DOWN;
                                                } else if (myTank == meRight) {
                                                     bullet[i].direction = RIGHT;
                                                } else {
                                                     bullet[i].direction = LEFT;
                                                }
                                                break;
                                            }
                                        }
                                       break;
                                }
                                break;
                            case PLAY_DUAL_GAME:
                                switch (e.key.keysym.sym) {
                                        case SDLK_DOWN:
                                            positive = 1;
                                            if (first_user == 1) {

                                                moveDown(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &controlRect);
                                                myTank = meDown;

                                                char directionMessage[BUFF_SIZE];
                                                strcpy(directionMessage, createDirectionMessage(in_room_id, myId, DOWN));
                                                bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                            } else {
                                                moveDown(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &friendRect);
                                                myTank = meDown;
                                                 char directionMessage[BUFF_SIZE];
                                                strcpy(directionMessage, createDirectionMessage(in_room_id, myId, DOWN));
                                                bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                            }
                                            break;
                                        case SDLK_UP:
                                            positive = 1;
                                            if (first_user == 1) {
                                                moveUp(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &controlRect);
                                                myTank = meUp;
                                                 char directionMessage[BUFF_SIZE];
                                                strcpy(directionMessage, createDirectionMessage(in_room_id, myId, UP));
                                                printf("check messgae: %s\n", directionMessage);
                                                bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);

                                            } else {
                                                moveUp(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &friendRect);
                                                myTank = meUp;
                                                 char directionMessage[BUFF_SIZE];
                                                strcpy(directionMessage, createDirectionMessage(in_room_id, myId, UP));
                                                printf("check messgae: %s\n", directionMessage);
                                                bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                            }
                                            break;
                                        case SDLK_RIGHT:
                                            positive = 1;
                                            if (first_user == 1) {
                                                moveRight(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &controlRect);
                                                myTank = meRight;
                                                 char directionMessage[BUFF_SIZE];
                                                strcpy(directionMessage, createDirectionMessage(in_room_id, myId, RIGHT));
                                                bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                            } else {
                                                moveRight(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &friendRect);
                                                myTank = meRight;
                                                 char directionMessage[BUFF_SIZE];
                                                strcpy(directionMessage, createDirectionMessage(in_room_id, myId, RIGHT));
                                                bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                            }
                                            break;
                                        case SDLK_LEFT:
                                            positive = 1;
                                            if (first_user == 1) {
                                                moveLeft(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &controlRect);
                                                myTank = meLeft;
                                                 char directionMessage[BUFF_SIZE];
                                                strcpy(directionMessage, createDirectionMessage(in_room_id, myId, LEFT));
                                                bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                            } else {
                                                moveLeft(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &friendRect);
                                                myTank = meLeft;
                                                 char directionMessage[BUFF_SIZE];
                                                strcpy(directionMessage, createDirectionMessage(in_room_id, myId, LEFT));
                                                bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                            }
                                            break;
                                    case SDLK_RETURN:
                                        break;
                                    case SDLK_SPACE:
                                        positive = 1;
                                        for (int i =0; i < totalBullets; i ++) {
                                            if (bullet[i].is_active == 0) {
                                                bullet[i].is_active = 1;
                                                 if (first_user == 1 && positive == 1) {
                                                    bulletRect[i].x = controlRect.x;
                                                    bulletRect[i].y = controlRect.y;
                                                } else if (first_user == 0 && positive == 1){
                                                    bulletRect[i].x = friendRect.x;
                                                    bulletRect[i].y = friendRect.y;
                                                }
                                                if (myTank == meUp) {
                                                    bullet[i].direction = UP;
                                                    char bulletDirectionMessage[BUFF_SIZE];
                                                    strcpy(bulletDirectionMessage, createBulletDirectionMessage(in_room_id, myId, UP));
                                                    bytes_sent = send(client_sock, bulletDirectionMessage, strlen(bulletDirectionMessage), 0);
                                                } else if (myTank == meDown) {
                                                     bullet[i].direction = DOWN;
                                                     char bulletDirectionMessage[BUFF_SIZE];
                                                    strcpy(bulletDirectionMessage, createBulletDirectionMessage(in_room_id, myId, DOWN));
                                                    bytes_sent = send(client_sock, bulletDirectionMessage, strlen(bulletDirectionMessage), 0);
                                                } else if (myTank == meRight) {
                                                     bullet[i].direction = RIGHT;
                                                     char bulletDirectionMessage[BUFF_SIZE];
                                                    strcpy(bulletDirectionMessage, createBulletDirectionMessage(in_room_id, myId, RIGHT));
                                                    bytes_sent = send(client_sock, bulletDirectionMessage, strlen(bulletDirectionMessage), 0);
                                                } else {
                                                     bullet[i].direction = LEFT;
                                                     char bulletDirectionMessage[BUFF_SIZE];
                                                    strcpy(bulletDirectionMessage, createBulletDirectionMessage(in_room_id, myId, LEFT));
                                                    bytes_sent = send(client_sock, bulletDirectionMessage, strlen(bulletDirectionMessage), 0);
                                                }
                                                break;
                                            }
                                        }
                                       break;
                                }
                                break;
                        }
                    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                        if (state == LOGIN) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            setFocusOnTextbox(&usernameLoginTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&passwordLoginTextbox, mouseX, mouseY);

                            if (mouseX >= 230 && mouseX <= 430 && mouseY >= 330 && mouseY <= 370) {
                                char loginMessage[BUFF_SIZE];
                                strcpy(loginMessage, createLoginMessage(usernameLoginTextbox.text, passwordLoginTextbox.text));

                                bytes_sent = send(client_sock, loginMessage, strlen(loginMessage), 0);
                                if (bytes_sent < 0) {
                                    perror("Error sending message: ");
                                }
                        
                                bytes_received = recv(client_sock, buff, sizeof(buff), 0);    
                                if (bytes_received < 0) {
                                    perror("Error: ");
                                    close(client_sock);
                                }
                                buff[bytes_received] = '\0';

                                if (strcmp(buff, "not found") == 0) {
                                   strcpy(login_message, "Account not found");
                                } else if (strcmp(buff, "locked") == 0){
                                    strcpy(login_message, "Your account was locked");
                                } else if (strcmp(buff, "logged") == 0) {
                                    strcpy(login_message, "Your account was logged somewhere");
                                } else if (strcmp(buff, "invalid") == 0) {
                                    strcpy(login_message, "username or password incorrect");
                                } else {
                                   state = CHOOSE_MODE;
                                   myId = atoi(buff);
                                }
                            }
                        } else if (state == REGISTER) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            setFocusOnTextbox(&usernameRegisterTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&passwordRegisterTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&retypePasswordRegisterTextbox, mouseX, mouseY);

                        } else if (state == CHOOSE_ROOM) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            if (mouseX >= 480 && mouseX <= 560 && mouseY >= 110 && mouseY <= 150) {
                                char message[BUFF_SIZE];

                                strcpy(message , createCreateRoomMessage(myId));
                                bytes_sent = send(client_sock, message, strlen(message), 0);
                                if (bytes_sent < 0) {
                                    perror("Error sending message: ");
                                }
                        
                                bytes_received = recv(client_sock, buff, sizeof(buff), 0);    
                                if (bytes_received < 0) {
                                    perror("Error: ");
                                    close(client_sock);
                                }
                                buff[bytes_received] = '\0';
                                in_room_id = atoi(buff);

                                rooms[in_room_id].status = 1;
                                rooms[in_room_id].first_player_id = myId;
                                state = WAITING_OTHER;
                                first_user = 1;
                                dual_mode_position_x = 4;
                                dual_mode_position_y = 11;
                                vertical_controller = 88;
                                hozirontal_controller = 32;

                                dual_mode_position_x_friend = 8;
                                dual_mode_position_y_friend = 11;
                                vertical_controller_friend = 88;
                                hozirontal_controller_friend = 64;

                                if (pthread_create(&tid, NULL, receiveThread, (void*)&client_sock) != 0) {
                                    perror("Error creating thread");
                                    return 1;
                                }

                                receiveThreadCreated = 1;  // Set the flag to indicate that the thread is created
                                pthread_detach(tid);
                            } else {
                                SDL_GetMouseState(&mouseX, &mouseY);
                                int row = (mouseY - 200) /100;
                                int col = (mouseX - 100) / 80;
                                dual_mode_position_x = 8;
                                dual_mode_position_y = 11;
                                vertical_controller = 88;
                                hozirontal_controller = 64;

                                dual_mode_position_x_friend = 4;
                                dual_mode_position_y_friend = 11;
                                vertical_controller_friend = 88;
                                hozirontal_controller_friend = 32;
                                
                                if (mouseX > 100 && mouseY > 200) {
                                    in_room_id = 1 + row*4 + col;
                                    
                                    if (in_room_id > 0 && in_room_id <= room_nums) {
                                        
                                        if (rooms[in_room_id].status == 1) {
                                            char joinRoomMessage[BUFF_SIZE];
                                            strcpy(joinRoomMessage, createJoinRoomMessage(myId, in_room_id));

                                            bytes_sent = send(client_sock, joinRoomMessage, strlen(joinRoomMessage), 0);
                                            if (bytes_sent < 0) {
                                                perror("Error sending message: ");
                                            }
                                            bytes_received = recv(client_sock, buff, sizeof(buff), 0);    
                                            if (bytes_received < 0) {
                                                perror("Error: ");
                                                close(client_sock);
                                            }

                                            buff[bytes_received] = '\0';

                                            if (strcmp(buff, "join success") == 0) {
                                                printf("join success");
                                                state = IN_ROOM;
                                                if (pthread_create(&tid, NULL, receiveThread, (void*)&client_sock) != 0) {
                                                    perror("Error creating thread");
                                                    return 1;
                                                }

                                                receiveThreadCreated = 1;  // Set the flag to indicate that the thread is created
                                                pthread_detach(tid);
                                                ready_to_start = 1;
                                            }
                                        }
                                    }
                                }

                                
                            }
                        } else if (state == IN_ROOM && ready_to_start == 1) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            printf("x: %d\n", mouseX);
                            printf("y: %d\n", mouseY);

                            if (mouseX > 220 && mouseX < 420 && mouseY > 370 && mouseY < 410) {
                                myTank = meUp;
                                state = PLAY_DUAL_GAME;

                                char startGameMessage[BUFF_SIZE];
                                strcpy(startGameMessage, createStartGameMessage(in_room_id));

                                bytes_sent = send(client_sock, startGameMessage, strlen(startGameMessage), 0);
                                if (bytes_sent < 0) {
                                    perror("Error sending message: ");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
 
    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

	close(client_sock);
	return 0;
}

void setFocusOnTextbox(Textbox *textbox, int mouseX, int mouseY) {
    // Check if the mouse click is within the textbox's rectangle
    if (mouseX >= textbox->boxRect.x && mouseX <= textbox->boxRect.x + textbox->boxRect.w &&
        mouseY >= textbox->boxRect.y && mouseY <= textbox->boxRect.y + textbox->boxRect.h) {
        textbox->hasFocus = true;  // Mouse click is inside the textbox
    } else {
        textbox->hasFocus = false; // Mouse click is outside the textbox
    }
}

void moveDown(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *controlRect) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (vertical_controller % 8 == 0 && *coor_y + 1 < 12 && single_map_1[*coor_y + 1][*coor_x] == 0) {
        if (horizontal_controller % 8 != 0 && horizontal_controller < *coor_x * 8) {
            if (single_map_1[*coor_y + 1][*coor_x - 1] == 0 && single_map_1[*coor_y + 1][*coor_x] == 0) {
                ++vertical_controller;
                controlRect->y += 5;
            }
        } else if (horizontal_controller % 8 != 0 && horizontal_controller > *coor_x * 8) {
            if (single_map_1[*coor_y + 1][*coor_x] == 0 && single_map_1[*coor_y + 1][*coor_x + 1] == 0) {
                ++vertical_controller;
                controlRect->y += 5;
            }
        } else if (horizontal_controller % 8 == 0) {
            ++vertical_controller;
            controlRect->y += 5;
        }
    } else if (vertical_controller % 8 == 7 && vertical_controller + 1 == (*coor_y + 1) * 8) {
        ++vertical_controller;
        controlRect->y += 5;
        updatePlayerPosition(coor_y, coor_x, 1, 0);
    } else if (vertical_controller % 8 != 0) {
        ++vertical_controller;
        controlRect->y += 5;
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void moveUp(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *controlRect) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    // Check if moving up is possible and handle the cases
    if (vertical_controller % 8 == 0 && *coor_y - 1 >= 0 && single_map_1[*coor_y - 1][*coor_x] == 0) {
        if (horizontal_controller % 8 != 0 && horizontal_controller < *coor_x * 8) {
            if (single_map_1[*coor_y - 1][*coor_x - 1] == 0 && single_map_1[*coor_y - 1][*coor_x] == 0) {
                --vertical_controller;
                controlRect->y -= 5;
            }
        } else if (horizontal_controller % 8 != 0 && horizontal_controller > *coor_x * 8) {
            if (single_map_1[*coor_y - 1][*coor_x] == 0 && single_map_1[*coor_y - 1][*coor_x + 1] == 0) {
                --vertical_controller;
                controlRect->y -= 5;
            }
        } else if (horizontal_controller % 8 == 0) {
            --vertical_controller;
            controlRect->y -= 5;
        }
    } else if (vertical_controller % 8 == 1 && vertical_controller - 1 == (*coor_y - 1) * 8) {
        --vertical_controller;
        controlRect->y -= 5;
        updatePlayerPosition(coor_y, coor_x, -1, 0);
    } else if (vertical_controller % 8 != 0) {
        --vertical_controller;
        controlRect->y -= 5;
        
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void moveRight(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *controlRect1) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    // Check if moving right is possible and handle the cases
    if (horizontal_controller % 8 == 0 && *coor_x + 1 < 12 && single_map_1[*coor_y][*coor_x + 1] == 0) {
        if (vertical_controller % 8 != 0 && vertical_controller < *coor_y * 8) {
            if (single_map_1[*coor_y][*coor_x + 1] == 0 && single_map_1[*coor_y - 1][*coor_x + 1] == 0) {
                ++horizontal_controller;
                controlRect1->x += 5;
            }
        } else if (vertical_controller % 8 != 0 && vertical_controller > *coor_y * 8) {
            if (single_map_1[*coor_y + 1][*coor_x + 1] == 0 && single_map_1[*coor_y][*coor_x + 1] == 0) {
                ++horizontal_controller;
                controlRect1->x += 5;
            }
        } else if (vertical_controller % 8 == 0) {
            ++horizontal_controller;
            controlRect1->x += 5;
        }
    } else if (horizontal_controller % 8 == 7 && horizontal_controller + 1 == (*coor_x + 1) * 8) {
        ++horizontal_controller;
        controlRect1->x += 5;
        updatePlayerPosition(coor_y, coor_x, 0, 1);
    } else if (horizontal_controller % 8 != 0) {
        ++horizontal_controller;
        controlRect1->x += 5;
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void moveLeft(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *controlRect) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;
    // Check if moving left is possible and handle the cases
    if (horizontal_controller % 8 == 0 && *coor_x - 1 >= 0 && single_map_1[*coor_y][*coor_x - 1] == 0) {
        if (vertical_controller % 8 != 0 && vertical_controller < *coor_y * 8) {
            if (single_map_1[*coor_y][*coor_x - 1] == 0 && single_map_1[*coor_y - 1][*coor_x - 1] == 0) {
                --horizontal_controller;
                controlRect->x -= 5;
            }
        } else if (vertical_controller % 8 != 0 && vertical_controller > *coor_y * 8) {
            if (single_map_1[*coor_y][*coor_x - 1] == 0 && single_map_1[*coor_y + 1][*coor_x - 1] == 0) {
                --horizontal_controller;
                controlRect->x -= 5;
            }
        } else if (vertical_controller % 8 == 0) {
            --horizontal_controller;
            controlRect->x -= 5;
        }
    } else if (horizontal_controller % 8 == 1 && horizontal_controller - 1 == (*coor_x - 1) * 8) {
        --horizontal_controller;
        controlRect->x -= 5;
        updatePlayerPosition(coor_y, coor_x, 0, -1);
    } else if (horizontal_controller % 8 != 0) {
        --horizontal_controller;
        controlRect->x -= 5;
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void extractAndChangeValues(const char *input, int *id, int *state, int *first_player_id, int *second_player_id) {
    if (input != NULL && id != NULL && state != NULL && first_player_id != NULL && second_player_id != NULL) {
        // Use sscanf to extract all four values from the input string
        int parsed = sscanf(input, "%d*%d*%d*%d", id, state, first_player_id, second_player_id);

        if (parsed != 4) {
            // Handle the case where parsing was unsuccessful (e.g., invalid input)
            // You can set default values or handle errors here as needed.
            *id = -1;
            *state = -1;
            *first_player_id = -1;
            *second_player_id = -1;
        } 
    }
}


void renderRooms(SDL_Renderer *renderer, SDL_Texture *roomTexture, TTF_Font *font) {
    int roomWidth = 100;
    int roomHeight = 80;
    int spacingX = 20; // Adjust the horizontal spacing between rooms
    int spacingY = 20; // Adjust the vertical spacing between rooms

    for (int i = 1; i <= room_nums; i++) {
        int row = (i - 1) / NUM_COLS;
        int col = (i - 1) % NUM_COLS;

        SDL_Rect roomRect = {
            100 + col * (roomWidth + spacingX),
            200 + row * (roomHeight + spacingY),
            roomWidth,
            roomHeight
        };

        SDL_RenderCopy(renderer, roomTexture, NULL, &roomRect);

        // Render room id in the top left corner
        char roomIdText[2]; // Adjust the size as needed
        snprintf(roomIdText, sizeof(roomIdText), "%d", rooms[i].id);

        SDL_Surface *textSurface = TTF_RenderText_Solid(font, roomIdText, (SDL_Color){0, 0, 0, 255});
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

        SDL_Rect textRect = {
            roomRect.x,
            roomRect.y,
            textWidth,
            textHeight
        };

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        char roomStatusText[2]; // Adjust the size as needed
        snprintf(roomStatusText, sizeof(roomIdText), "%d", rooms[i].status);

        TTF_Font *roomStateFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);

        SDL_Surface *roomStatusTextSurface = TTF_RenderText_Solid(roomStateFont, roomStatusText, (SDL_Color){0, 0, 0, 255});
        SDL_Texture *roomStatusTextTexture = SDL_CreateTextureFromSurface(renderer, roomStatusTextSurface);
        SDL_FreeSurface(roomStatusTextSurface);

        SDL_Rect roomStatusRect = {
            roomRect.x+18,
            roomRect.y+25,
            roomStatusTextSurface->w,
            roomStatusTextSurface->h
        };

        SDL_RenderCopy(renderer, roomStatusTextTexture, NULL, &roomStatusRect);
        SDL_Texture *personTexture = IMG_LoadTexture(renderer, "images/person.jpg");

        SDL_Rect personRect = {
            roomRect.x+35,
            roomRect.y+25,
            15,15
        };

        SDL_RenderCopy(renderer, personTexture, NULL, &personRect);
    }
}


void checkRooms(){
    for (int i = 1; i <= room_nums; i ++) {
        printf("%d %d %d %d\n", rooms[i].id, rooms[i].first_player_id, rooms[i].second_player_id, rooms[i].status);
    }
}

void* receiveThread(void* arg) {
    int client_sock = *((int*)arg);
    char buff[BUFF_SIZE];
    int bytes_received;

    while (1) {
        // Receive messages from the server
        bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("\nConnection closed!\n");
            break;
        }
        buff[bytes_received] = '\0';
        printf("check: %s\n", buff);
        // Print received messages only when the chat has started
        if (strcmp(buff, "other joined") == 0) {
            ready_to_start = 1;
            state = IN_ROOM;
        } else if (strcmp(buff, "start game") == 0) {
            state = PLAY_DUAL_GAME;
        } else if (buff[0] == 0x07) {
            positive = 0;
            int direction;
            sscanf(buff + 1, "%d", &direction); // Skip the first byte (0x05) and read the integer.

            if (first_user == 1) {
                
                if (direction == UP) {
                    friendTank = meUp;
                    moveUp(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &friendRect);
                } else if (direction == DOWN) {
                    friendTank = meDown;
                    moveDown(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &friendRect);
                } else if (direction == RIGHT) {
                    friendTank = meRight;
                    moveRight(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &friendRect);
                } else {
                    friendTank = meLeft;
                    moveLeft(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &friendRect);
                }
            } else {
                if (direction == UP) {
                    friendTank = meUp;
                    moveUp(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &controlRect);
                } else if (direction == DOWN) {
                    friendTank = meDown;
                    moveDown(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &controlRect);
                } else if (direction == RIGHT) {
                    friendTank = meRight;
                    moveRight(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &controlRect);
                } else {
                    friendTank = meLeft;
                    moveLeft(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &controlRect);
                }
            }
        } else if (buff[0] == 0x09) {
            positive = 0;
            int direction;

            sscanf(buff + 1, "%d", &direction); // Skip the first byte (0x05) and read the integer.
           for (int i =0; i < totalBullets; i ++) {
                if (bullet[i].is_active == 0) {
                    bullet[i].is_active = 1;
                    
                    if (first_user == 1 && positive == 0) {
                        bulletRect[i].x = friendRect.x;
                        bulletRect[i].y = friendRect.y;
                    } else {
                        bulletRect[i].x = controlRect.x;
                        bulletRect[i].y = controlRect.y;
                    }
                    if (direction == UP) {
                            bullet[i].direction = UP;
                    } else if (direction == DOWN) {
                            bullet[i].direction = DOWN;
                    } else if (direction == RIGHT) {
                            bullet[i].direction = RIGHT;
                    } else {
                            bullet[i].direction = LEFT;
                    }
                    break;
                }
            }
        }   
    }

    pthread_exit(NULL);
}

void renderDualModeGame(SDL_Renderer *renderer, SDL_Texture *myTank, SDL_Texture *friendTank, SDL_Texture* bulletTexture[]) {
    // Set the background color to black
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    SDL_RenderClear(renderer);

    // Set the draw color for the gray border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // Calculate the map boundaries
    int mapX = (640 - (12 * 40)) / 2;
    int mapY = (480 - (12 * 40)) / 2;

    // Render the gray border around the map
    SDL_Rect borderRect = { mapX - 2, mapY - 2, 12 * 40 + 4, 12 * 40 + 4 };
    SDL_RenderFillRect(renderer, &borderRect);

    // Render the map
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 12; x++) {
            SDL_Rect destRect = { mapX + x * 40, mapY + y * 40, 40, 40 };
            SDL_Texture* texture = NULL;

            switch (dual_map_1[y][x]) {
                case 0:
                    break;
                case 1:
                    texture = block;
                    break;
                case 2:
                    texture = iron;
                    break;
                case 9:
                    if (first_user == 1) {
                        texture = myTank;
                        SDL_RenderCopy(renderer, texture, NULL, &controlRect);
                    } else {
                        texture = friendTank;
                        SDL_RenderCopy(renderer, texture, NULL, &controlRect);
                    }
                    break;
                case 8:
                    if (first_user == 0) {
                        texture = myTank;
                        SDL_RenderCopy(renderer, texture, NULL, &friendRect);
                    } else {
                        texture = friendTank;
                        SDL_RenderCopy(renderer, texture, NULL, &friendRect);
                    }
                    break;
                case 7:
                    texture = enermy_1_right;
                    break;
                default:
                    break;
            }

            if (texture != myTank) { 
                SDL_RenderCopy(renderer, texture, NULL, &destRect);
            }
        }
    }

     for (int i = 0; i < totalBullets; i ++) {
        if (bullet[i].is_active == 1) {
            SDL_RenderCopy(renderer, bulletTexture[i], NULL, &bulletRect[i]);
        }
        if (bullet[i].is_active == 1) {
            if (bullet[i].direction == UP) {
                // bullet[i].position_y = bulletRect[i].y / 5;
                if (dual_map_1[(bulletRect[i].y)/40 - 1][(bulletRect[i].x-80)/40] != 0 && bulletRect[i].y % 40 == 0) {
                    bullet[i].is_active = 0;
                    SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
                } else {
                    bulletRect[i].y -= 1;
                }
            } else if (bullet[i].direction == DOWN) {
                if (dual_map_1[(bulletRect[i].y)/40 + 1][(bulletRect[i].x-80)/40] != 0 && bulletRect[i].y % 40 == 0) {
                    bullet[i].is_active = 0;
                    SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
                }
                bulletRect[i].y += 1;
                // bullet[i].position_y = bulletRect[i].y / 5;
            } else if (bullet[i].direction == RIGHT) {
                if (dual_map_1[(bulletRect[i].y)/40][(bulletRect[i].x-80)/40 + 1] != 0 && bulletRect[i].x % 40 == 0) {
                    bullet[i].is_active = 0;
                    SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
                }
                bulletRect[i].x += 1;
                // bullet[i].position_x = bulletRect[i].x / 5;
            } else {
                if (dual_map_1[(bulletRect[i].y)/40][(bulletRect[i].x-80)/40 - 1] != 0 && bulletRect[i].x % 40 == 0) {
                    bullet[i].is_active = 0;
                    SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
                }
                bulletRect[i].x -= 1;
                // bullet[i].position_x = bulletRect[i].x / 5;
            }
        }
    }
}


            // SDL

// //-------------------Render enermy----------------------------------------------------------------------------------------------
//             for (int i = 0; i < 5; i++) {
//                 int direction;
//                 // Determine the direction for the enemy's movement (0: up, 1: down, 2: left, 3: right)
//                 do {
//                     direction = rand() % 4;
//                 } while (direction == previousDirection[i]);
//                 previousDirection[i] = direction;
//                 // Update the enermyRect based on the chosen direction
//                 switch (direction) {
//                     case 0: // Move up               
//                         if (enermyRect[i].y % 8 == 0 && enermy[i].position_y -1 >= 0 && dual_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0) {
//                             if (enermyRect[i].x % 8 != 0 && enermyRect[i].x < enermy[i].position_x * 8) {
//                                 if (dual_map_1[enermy[i].position_y - 1][enermy[i].position_x - 1] == 0 && dual_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0) {
//                                     enermyRect[i].y -= 5;
//                                 }
//                             } else if (enermyRect[i].x % 8 != 0 && enermyRect[i].x > enermy[i].position_x * 8) {
//                                 if (dual_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0 && dual_map_1[enermy[i].position_y - 1][enermy[i].position_x + 1] == 0) {
//                                     enermyRect[i].y -= 5;
//                                 }
//                             } else if (enermyRect[i].x % 8 == 0) {
//                                 enermyRect[i].y -= 5;
//                             }
//                         } else if (enermyRect[i].y % 8 == 7 && enermyRect[i].y + 1 == (enermy[i].position_y + 1) * 8) {
//                             enermyRect[i].y -= 5;
//                             enermy[i].position_y--;
//                         } else if (enermyRect[i].y % 8 != 0) {
//                             enermyRect[i].y -= 5;
//                         }
//                         break;
//                     case 1: // Move down
//                         if (enermyRect[i].y % 8 == 0 && enermy[i].position_y + 1< 12 && dual_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0) {
//                             if (enermyRect[i].x % 8 != 0 && enermyRect[i].x < enermy[i].position_x * 8) {
//                                 if (dual_map_1[enermy[i].position_y + 1][enermy[i].position_x - 1] == 0 && dual_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0) {
//                                     enermyRect[i].y+=5;
//                                 }
//                             } else if (enermyRect[i].x % 8 != 0 && enermyRect[i].x > enermy[i].position_x * 8) {
//                                 if (dual_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0 && dual_map_1[enermy[i].position_y + 1][enermy[i].position_x + 1] == 0) {
//                                     enermyRect[i].y+=5;
//                                 }
//                             } else if (enermyRect[i].x % 8 == 0) {
//                                 enermyRect[i].y+=5;
//                             }
//                         } else if (enermyRect[i].y % 8 == 1 && enermyRect[i].y - 1 == (enermy[i].position_y - 1) * 8) {
//                             enermyRect[i].y+=5;
//                             enermy[i].position_y++;
//                         } else if (enermyRect[i].y % 8 != 0) {
//                             enermyRect[i].y+=5;
//                         }
//                     break;
//                     case 2: // Move left
//                         if (enermyRect[i].x % 8 == 0 && enermy[i].position_x - 1 >= 0 && dual_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0) {
//                             if (enermyRect[i].y % 8 != 0 && enermyRect[i].y < enermy[i].position_y * 8) {
//                                 if (dual_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0 && dual_map_1[enermy[i].position_y - 1][enermy[i].position_x - 1] == 0) {
//                                     enermyRect[i].x-=5;
//                                 }
//                             } else if (enermyRect[i].y % 8 != 0 && enermyRect[i].y > enermy[i].position_y * 8) {
//                                 if (dual_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0 && dual_map_1[enermy[i].position_y + 1][enermy[i].position_x - 1] == 0) {
//                                     enermyRect[i].x-=5;
//                                 }
//                             } else if (enermyRect[i].y % 8 == 0) {
//                                 enermyRect[i].x-=5;
//                             }
//                         } else if (enermyRect[i].x % 8 == 1 && enermyRect[i].x - 1 == (enermy[i].position_x - 1) * 8) {
//                             enermyRect[i].x-=5;
//                             enermy[i].position_x--;
//                         } else if (enermyRect[i].x % 8 != 0) {
//                             enermyRect[i].x-=5;
//                         }
//                         break;
//                     case 3: // Move right
//                     if (enermyRect[i].x % 8 == 0 && enermy[i].position_x + 1 < 12 && dual_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0) {
//                         if (enermyRect[i].y % 8 != 0 && enermyRect[i].y < enermy[i].position_y * 8) {
//                             if (dual_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0 && dual_map_1[enermy[i].position_y - 1][enermy[i].position_x + 1] == 0) {
//                                 enermyRect[i].x+=5;
//                             }
//                         } else if (enermyRect[i].y % 8 != 0 && enermyRect[i].y > enermy[i].position_y * 8) {
//                             if (dual_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0 && dual_map_1[enermy[i].position_y + 1][enermy[i].position_x + 1] == 0) {
//                                 enermyRect[i].x+=5;
//                             }
//                         } else if (enermyRect[i].y % 8 == 0) {
//                             enermyRect[i].x+=5;
//                         }
//                     } else if (enermyRect[i].x % 8 == 7 && enermyRect[i].x + 1 == (enermy[i].position_x + 1) * 8) {
//                         enermyRect[i].x+=5;
//                         enermy[i].position_x++;
//                     } else if (enermyRect[i].x % 8 != 7) {
//                         enermyRect[i].x+=5;
//                     }
//                     break;
//                     default:
//                         break;
//                 }

//             }
//             for (int i = 0; i < 5; i++) {
//                 SDL_RenderCopy(renderer, enermyTexture[i], NULL, &enermyRect[i]);
//             }

// ///---------------------end render enermy-----------------------------------------------------------
//     for (int i = 0; i < totalBullets; i ++) {
//         if (bullet[i].is_active == 1) {
//             SDL_RenderCopy(renderer, bulletTexture[i], NULL, &bulletRect[i]);
//         }
//         if (bullet[i].is_active == 1) {
//             if (bullet[i].direction == UP) {
//                 // bullet[i].position_y = bulletRect[i].y / 5;
//                 if (dual_map_1[(bulletRect[i].y)/40 - 1][(bulletRect[i].x-80)/40] != 0 && bulletRect[i].y % 40 == 0) {
//                     bullet[i].is_active = 0;
//                     SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
//                 } else {
//                     bulletRect[i].y -= 1;
//                 }
//             } else if (bullet[i].direction == DOWN) {
//                 if (dual_map_1[(bulletRect[i].y)/40 + 1][(bulletRect[i].x-80)/40] != 0 && bulletRect[i].y % 40 == 0) {
//                     bullet[i].is_active = 0;
//                     SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
//                 }
//                 bulletRect[i].y += 1;
//                 // bullet[i].position_y = bulletRect[i].y / 5;
//             } else if (bullet[i].direction == RIGHT) {
//                 if (dual_map_1[(bulletRect[i].y)/40][(bulletRect[i].x-80)/40 + 1] != 0 && bulletRect[i].x % 40 == 0) {
//                     bullet[i].is_active = 0;
//                     SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
//                 }
//                 bulletRect[i].x += 1;
//                 // bullet[i].position_x = bulletRect[i].x / 5;
//             } else {
//                 if (dual_map_1[(bulletRect[i].y)/40][(bulletRect[i].x-80)/40 - 1] != 0 && bulletRect[i].x % 40 == 0) {
//                     bullet[i].is_active = 0;
//                     SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
//                 }
//                 bulletRect[i].x -= 1;
//                 // bullet[i].position_x = bulletRect[i].x / 5;
//             }
//         }
//     }
// }