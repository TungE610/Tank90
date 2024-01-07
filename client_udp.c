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
#include "messages.h"
#include "maps.h"
#include "variables.h"
#include "utils.h"
#include "fonts.h"
#include "textures.h"
#include "surfaces.h"
#include "colors.h"

int myId = 0;
int in_room_id = 0;
int foundRoomState = 1;
int room_nums = 0;
int ready_to_start = 0;

int positive = 1;
SDL_Renderer *renderer;

Room rooms[50];

int first_user = 0;

void initRoom(Room *room, int id, int status, int first_player_id, int second_player_id) {
    room->id = id;
    room->status = status;
    room->first_player_id = first_player_id;
    room->second_player_id = second_player_id;
}

void setFocusOnTextbox(Textbox *textbox, int mouseX, int mouseY);
void moveDown(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect, int stepEachBlock, int stepSize);
void moveUp(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect, int stepEachBlock, int stepSize);
void moveRight(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect, int stepEachBlock, int stepSize);
void moveLeft(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect, int stepEachBlock, int stepSize);
void extractAndChangeValues(const char *input, int *id, int *state, int *first_player_id, int *second_player_id, int stepEachBlock, int stepSize);
void renderRooms(SDL_Renderer *renderer, SDL_Texture *roomTexture, TTF_Font *font);
void checkRooms();
void* receiveThread(void* arg);
void renderDualModeGame(SDL_Renderer *renderer, SDL_Texture *myTank, SDL_Texture *friendTank, SDL_Texture* bulletTexture[]);

Bullet bullet[totalBullets];
SDL_Rect bulletRect[totalBullets];

int main(){
    TTF_Init();
    initFonts();
    Uint32 prevFrameTime = SDL_GetTicks();

	int client_sock;
	unsigned char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent,bytes_received, sin_size;

    int menuChoose = 1;
    int chooseMode = 1;
    int single_mode_stage = 1;

    strcpy(login_message, " ");
    strcpy(register_message, " ");
    int mouseX, mouseY;
    pthread_t tid;
    int receiveThreadCreated = 0;
    
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

            screenSurface = SDL_GetWindowSurface( window );

            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0xFF, 0xFF ) );
            
            SDL_UpdateWindowSurface( window );

            SDL_Event e; bool quit = false;
            IMG_Init(IMG_INIT_PNG);

            loadSurface();

            // Create a texture from the text surface
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);      
            SDL_Texture *loginTextTexture = SDL_CreateTextureFromSurface(renderer, loginText);      
            SDL_Texture *registerTextTexture = SDL_CreateTextureFromSurface(renderer, registerText);
            
            SDL_Rect renderQuad = { 150, 150, textSurface->w, textSurface->h };
            SDL_Rect loginTextRenderQuad = { 220, 230, loginText->w, loginText->h };
            SDL_Rect registerTextRenderQuad = { 220, 280, registerText->w, registerText->h };

            loadTextures(renderer);
            SDL_Rect tankRect = {180, 225, TANK_WIDTH, TANK_HEIGHT};

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);  // Black background
            SDL_RenderClear(renderer);

            // // Render the text texture
            SDL_Rect loginBigTextRenderQuad = { 195, 100, loginBigTextSurface->w, loginBigTextSurface->h };
            SDL_Rect roomsBigTextRenderQuad = { 195, 100, roomsBigText->w, roomsBigText->h };
            SDL_Rect usernameRenderQuad = { 120, 200, usernameText->w, usernameText->h };
            SDL_Rect passwordRenderQuad = { 120, 250, passwordText->w, passwordText->h };
            SDL_Rect retypePasswordRenderQuad = { 120, 300, retypePasswordText->w, retypePasswordText->h };
            SDL_Rect chooseModeRenderQuad = { 50, 100, chooseModeText->w, chooseModeText->h };
            SDL_Rect singleModeRenderQuad = { 250, 250, singleModeText->w, singleModeText->h };
            SDL_Rect dualModeRenderQuad = { 250, 300, dualModeText->w, dualModeText->h };
            SDL_Rect tankRectChooseMode = {200, 242, TANK_WIDTH, TANK_HEIGHT};

            initTextbox(&usernameLoginTextbox, 320, 195, 200, 30);
            initTextbox(&passwordLoginTextbox, 320, 245, 200, 30);
            
            initTextbox(&usernameRegisterTextbox, 300, 195, 200, 30);
            initTextbox(&passwordRegisterTextbox, 300, 245, 200, 30);
            initTextbox(&retypePasswordRegisterTextbox, 410, 295, 200, 30);

            // // Initialize bullets and rects
            // for (int i = 0; i < totalBullets; i++) {
            //     bullet[i].position_x = hozirontal_controller;
            //     bullet[i].position_y = vertical_controller;
            //     bullet[i].is_active = 0;  // Assuming first bullet is active
            //     bulletRect[i] = (SDL_Rect){ controlRect.x, controlRect.y, 30, 30 };
            // }

            // Enermy enermy[5];
            // SDL_Rect enermyRect[5];

            // for (int i = 0; i < 5; i ++) {
            //     enermy[i].position_x = 11;
            //     enermy[i].position_y = 0;
            //     enermy[i].direction = LEFT;
            //     enermy[i].blood = 1;
            //     enermyRect[i] = (SDL_Rect){ 440, 0, 40, 40 };
            //     enermy[i].movementTimer = 0.0f; // Initialize movement timer to zero
            // }

            while( quit == false ){

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);  // Black background
                    SDL_RenderClear(renderer);
//                 SDL_Texture *bulletTexture[totalBullets];
//                 for (int i = 0; i < totalBullets; i ++) {
//                     if (bullet[i].direction == UP) {
//                         bulletTexture[i] = IMG_LoadTexture(renderer, "./images/bullet_up.png");
//                     } else if (bullet[i].direction == DOWN) {
//                         bulletTexture[i] = IMG_LoadTexture(renderer, "./images/bullet_down.png");
//                     } else if (bullet[i].direction == RIGHT) {
//                         bulletTexture[i] = IMG_LoadTexture(renderer, "./images/bullet_right.png");
//                     } else {
//                         bulletTexture[i] = IMG_LoadTexture(renderer, "./images/bullet_left.png");
//                     }
                    
//                     if (bullet[i].is_active == 0) {
//                         bulletRect[i].x  = controlRect.x;
//                         bulletRect[i].y  = controlRect.y;
//                     }
//                 }

//                 SDL_Texture *enermyTexture[5];

//                 for (int i = 0; i < 5; i ++) {
//                     if (enermy[i].direction == UP) {
//                         enermyTexture[i] = IMG_LoadTexture(renderer, "./images/enermy_up.png");
//                     } else if (enermy[i].direction == DOWN) {
//                         enermyTexture[i] = IMG_LoadTexture(renderer, "./images/enermy_down.png");
//                     } else if (enermy[i].direction == RIGHT) {
//                         enermyTexture[i] = IMG_LoadTexture(renderer, "./images/enermy_right.png");
//                     } else {
//                         enermyTexture[i] = IMG_LoadTexture(renderer, "./images/enermy_left.png");
//                     }
//                 }

//                 SDL_RenderClear(renderer);
                if (state == MAIN_MENU) {
                    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
                    SDL_RenderCopy(renderer, loginTextTexture, NULL, &loginTextRenderQuad);
                    SDL_RenderCopy(renderer, registerTextTexture, NULL, &registerTextRenderQuad);
                    SDL_RenderCopy(renderer, tank, NULL, &tankRect);
                }
// //------------------------------------------------------------------------------------------------------------------------------------------
                else if (state == LOGIN) {
                    SDL_RenderCopy(renderer, loginBigTextTexture, NULL, &loginBigTextRenderQuad);
                    SDL_RenderCopy(renderer, usernameTextTexture, NULL, &usernameRenderQuad);
                    SDL_RenderCopy(renderer, passwordTextTexture, NULL, &passwordRenderQuad);
                    
                    SDL_Surface *loginMessgageText = TTF_RenderText_Solid(MINI_FONT, login_message, RED);
                    SDL_Texture *loginMessageTextTexture = SDL_CreateTextureFromSurface(renderer, loginMessgageText);   
                    SDL_Rect loginMessageRenderQuad = { 120, 285, loginMessgageText->w, loginMessgageText->h }; 
                    SDL_RenderCopy(renderer, loginMessageTextTexture, NULL, &loginMessageRenderQuad);

                    SDL_Surface *usernameBoxTextSurface = TTF_RenderText_Solid(TINY_FONT, usernameLoginTextbox.text, WHITE);
                    SDL_Texture *usernameBoxTextTexture = SDL_CreateTextureFromSurface(renderer, usernameBoxTextSurface);
                    int usernameTextWidth = usernameBoxTextSurface->w;
                    int usernameTextHeight = usernameBoxTextSurface->h;
                    SDL_Rect usernameBoxRenderQuad = { usernameLoginTextbox.boxRect.x + 5, usernameLoginTextbox.boxRect.y + 5, usernameTextWidth, usernameTextHeight };

                    SDL_Surface *passwordBoxTextSurface = TTF_RenderText_Solid(TINY_FONT, passwordLoginTextbox.text, WHITE);
                    SDL_Texture *passwordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, passwordBoxTextSurface);
                    int passwordTextWidth = passwordBoxTextSurface->w;
                    int passwordTextHeight = passwordBoxTextSurface->h;
                    SDL_Rect passwordBoxRenderQuad = { passwordLoginTextbox.boxRect.x + 5, passwordLoginTextbox.boxRect.y + 5, passwordTextWidth, passwordTextHeight };

                    SDL_RenderCopy(renderer, usernameBoxTextTexture, NULL, &usernameBoxRenderQuad);
                    SDL_RenderCopy(renderer, passwordBoxTextTexture, NULL, &passwordBoxRenderQuad);

                    initButton(&loginButton, 230, 330, 200, 40, "Login");
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 20);
                    SDL_Rect loginButtonRect = { loginButton.boxRect.x, loginButton.boxRect.y, loginButton.boxRect.w, loginButton.boxRect.h };
                    SDL_RenderFillRect(renderer, &loginButtonRect);
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);

                    SDL_Surface *buttonTextSurface = TTF_RenderText_Solid(TINY_FONT, loginButton.text, WHITE);
                    SDL_Texture *buttonTextTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);

                    SDL_Rect buttonTextRenderQuad = { loginButton.boxRect.x + 50, loginButton.boxRect.y + 12, buttonTextSurface->w, buttonTextSurface->h };
                    SDL_RenderCopy(renderer, buttonTextTexture, NULL, &buttonTextRenderQuad);

                } 
//-----------------------------------------------------------------------------------------------------------------------------------------------
                else if (state == REGISTER) {
                    SDL_RenderCopy(renderer, registerBigTextTexture, NULL, &loginBigTextRenderQuad);
                    SDL_RenderCopy(renderer, usernameTextTexture, NULL, &usernameRenderQuad);
                    SDL_RenderCopy(renderer, passwordTextTexture, NULL, &passwordRenderQuad);
                    SDL_RenderCopy(renderer, retypePasswordTextTexture, NULL, &retypePasswordRenderQuad);

                    SDL_Surface *registerMessgageText = TTF_RenderText_Solid(MINI_FONT, register_message, RED);
                    SDL_Texture *registerMessageTextTexture = SDL_CreateTextureFromSurface(renderer, registerMessgageText);   
                    SDL_Rect registerMessageRenderQuad = { 120, 335, registerMessgageText->w, registerMessgageText->h }; 
                    SDL_RenderCopy(renderer, registerMessageTextTexture, NULL, &registerMessageRenderQuad);

                    SDL_Surface *usernameBoxTextSurface = TTF_RenderText_Solid(TINY_FONT, usernameRegisterTextbox.text, WHITE);
                    SDL_Texture *usernameBoxTextTexture = SDL_CreateTextureFromSurface(renderer, usernameBoxTextSurface);

                    int usernameTextWidth = usernameBoxTextSurface->w;
                    int usernameTextHeight = usernameBoxTextSurface->h;
                    SDL_Rect usernameBoxRenderQuad = { usernameRegisterTextbox.boxRect.x + 5, usernameRegisterTextbox.boxRect.y + 5, usernameTextWidth, usernameTextHeight };
                    SDL_Surface *passwordBoxTextSurface = TTF_RenderText_Solid(TINY_FONT, passwordRegisterTextbox.text, WHITE);
                    SDL_Texture *passwordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, passwordBoxTextSurface);

                    int passwordTextWidth = passwordBoxTextSurface->w;
                    int passwordTextHeight = passwordBoxTextSurface->h;
                    SDL_Rect passwordBoxRenderQuad = { passwordRegisterTextbox.boxRect.x + 5, passwordRegisterTextbox.boxRect.y + 5, passwordTextWidth, passwordTextHeight };
                    SDL_Rect showPasswordRenderQuad = {passwordRegisterTextbox.boxRect.x + 300, passwordRegisterTextbox.boxRect.y-2, 28, 28};
                    SDL_Rect showRetypedPasswordRenderQuad = {passwordRegisterTextbox.boxRect.x + 300, passwordRegisterTextbox.boxRect.y+60, 28, 28};
                    
                    SDL_Surface *retypePasswordRegisterBoxTextSurface = TTF_RenderText_Solid(TINY_FONT, retypePasswordRegisterTextbox.text, WHITE);
                    SDL_Texture *retypePasswordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, retypePasswordRegisterBoxTextSurface);

                    int retypePasswordTextWidth = retypePasswordRegisterBoxTextSurface->w;
                    int retypePasswordTextHeight = retypePasswordRegisterBoxTextSurface->h;
                    SDL_Rect retypePasswordBoxRenderQuad = { retypePasswordRegisterTextbox.boxRect.x + 5, retypePasswordRegisterTextbox.boxRect.y + 5, retypePasswordTextWidth, retypePasswordTextHeight };

                    SDL_RenderCopy(renderer, usernameBoxTextTexture, NULL, &usernameBoxRenderQuad);
                    SDL_RenderCopy(renderer, passwordBoxTextTexture, NULL, &passwordBoxRenderQuad);
                    SDL_RenderCopy(renderer, retypePasswordBoxTextTexture, NULL, &retypePasswordBoxRenderQuad);

                    if (showPassword == 0) {
                        SDL_RenderCopy(renderer, hidePasswordTexture, NULL, &showPasswordRenderQuad);
                    } else {
                        SDL_RenderCopy(renderer, showPasswordTexture, NULL, &showPasswordRenderQuad);
                    }

                    if (showRetypePassword == 0) {
                        SDL_RenderCopy(renderer, hidePasswordTexture, NULL, &showRetypedPasswordRenderQuad);
                    } else {
                        SDL_RenderCopy(renderer, showPasswordTexture, NULL, &showRetypedPasswordRenderQuad);
                    }

                    initButton(&regiterButton, 230, 380, 200, 40, "Register");

                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 20); // Blue color
                    SDL_Rect regiterButtonRect = { regiterButton.boxRect.x, regiterButton.boxRect.y, regiterButton.boxRect.w, regiterButton.boxRect.h };
                    SDL_RenderFillRect(renderer, &regiterButtonRect);

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25); // Black background
                    SDL_Surface *buttonTextSurface = TTF_RenderText_Solid(TINY_FONT, regiterButton.text, WHITE);
                    SDL_Texture *buttonTextTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);
                    SDL_Rect buttonTextRenderQuad = { regiterButton.boxRect.x + 20, regiterButton.boxRect.y + 12, buttonTextSurface->w, buttonTextSurface->h };
                    SDL_RenderCopy(renderer, buttonTextTexture, NULL, &buttonTextRenderQuad);
                }
// //-------------------------------------------------------------------------------------------------------------------------------------------
                else if (state == CHOOSE_MODE) {
                    SDL_RenderCopy(renderer, chooseModeTextTexture, NULL, &chooseModeRenderQuad);
                    SDL_RenderCopy(renderer, singleModeTextTexture, NULL, &singleModeRenderQuad);
                    SDL_RenderCopy(renderer, dualModeTextTexture, NULL, &dualModeRenderQuad);
                    SDL_RenderCopy(renderer, tank, NULL, &tankRectChooseMode);
                }
                else if (state == PLAY_SINGLE_GAME) {

                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                renderSingleMap1(renderer);

//-------------------Render enermy----------------------------------------------------------------------------------------------
//                         for (int i = 0; i < 5; i++) {
//                             int direction;
//                             // Determine the direction for the enemy's movement (0: up, 1: down, 2: left, 3: right)
//                             do {
//                                 direction = rand() % 4;
//                             } while (direction == previousDirection[i]);
//                             previousDirection[i] = direction;
//                             // Update the enermyRect based on the chosen direction
//                             switch (direction) {
//                                 case 0: // Move up               
//                                     if (enermyRect[i].y % 8 == 0 && enermy[i].position_y -1 >= 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0) {
//                                         if (enermyRect[i].x % 8 != 0 && enermyRect[i].x < enermy[i].position_x * 8) {
//                                             if (single_map_1[enermy[i].position_y - 1][enermy[i].position_x - 1] == 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0) {
//                                                 enermyRect[i].y -= 5;
//                                             }
//                                         } else if (enermyRect[i].x % 8 != 0 && enermyRect[i].x > enermy[i].position_x * 8) {
//                                             if (single_map_1[enermy[i].position_y - 1][enermy[i].position_x] == 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x + 1] == 0) {
//                                                 enermyRect[i].y -= 5;
//                                             }
//                                         } else if (enermyRect[i].x % 8 == 0) {
//                                             enermyRect[i].y -= 5;
//                                         }
//                                     } else if (enermyRect[i].y % 8 == 7 && enermyRect[i].y + 1 == (enermy[i].position_y + 1) * 8) {
//                                         enermyRect[i].y -= 5;
//                                         enermy[i].position_y--;
//                                     } else if (enermyRect[i].y % 8 != 0) {
//                                         enermyRect[i].y -= 5;
//                                     }
//                                     break;
//                                case 1: // Move down
//                                     if (enermyRect[i].y % 8 == 0 && enermy[i].position_y + 1< 12 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0) {
//                                         if (enermyRect[i].x % 8 != 0 && enermyRect[i].x < enermy[i].position_x * 8) {
//                                             if (single_map_1[enermy[i].position_y + 1][enermy[i].position_x - 1] == 0 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0) {
//                                                 enermyRect[i].y+=5;
//                                             }
//                                         } else if (enermyRect[i].x % 8 != 0 && enermyRect[i].x > enermy[i].position_x * 8) {
//                                             if (single_map_1[enermy[i].position_y + 1][enermy[i].position_x] == 0 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x + 1] == 0) {
//                                                 enermyRect[i].y+=5;
//                                             }
//                                         } else if (enermyRect[i].x % 8 == 0) {
//                                             enermyRect[i].y+=5;
//                                         }
//                                     } else if (enermyRect[i].y % 8 == 1 && enermyRect[i].y - 1 == (enermy[i].position_y - 1) * 8) {
//                                         enermyRect[i].y+=5;
//                                         enermy[i].position_y++;
//                                     } else if (enermyRect[i].y % 8 != 0) {
//                                         enermyRect[i].y+=5;
//                                     }
//                                 break;
//                                 case 2: // Move left
//                                     if (enermyRect[i].x % 8 == 0 && enermy[i].position_x - 1 >= 0 && single_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0) {
//                                         if (enermyRect[i].y % 8 != 0 && enermyRect[i].y < enermy[i].position_y * 8) {
//                                             if (single_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x - 1] == 0) {
//                                                 enermyRect[i].x-=5;
//                                             }
//                                         } else if (enermyRect[i].y % 8 != 0 && enermyRect[i].y > enermy[i].position_y * 8) {
//                                             if (single_map_1[enermy[i].position_y][enermy[i].position_x - 1] == 0 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x - 1] == 0) {
//                                                 enermyRect[i].x-=5;
//                                             }
//                                         } else if (enermyRect[i].y % 8 == 0) {
//                                             enermyRect[i].x-=5;
//                                         }
//                                     } else if (enermyRect[i].x % 8 == 1 && enermyRect[i].x - 1 == (enermy[i].position_x - 1) * 8) {
//                                         enermyRect[i].x-=5;
//                                         enermy[i].position_x--;
//                                     } else if (enermyRect[i].x % 8 != 0) {
//                                         enermyRect[i].x-=5;
//                                     }
//                                     break;
//                                case 3: // Move right
//                                 if (enermyRect[i].x % 8 == 0 && enermy[i].position_x + 1 < 12 && single_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0) {
//                                     if (enermyRect[i].y % 8 != 0 && enermyRect[i].y < enermy[i].position_y * 8) {
//                                         if (single_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0 && single_map_1[enermy[i].position_y - 1][enermy[i].position_x + 1] == 0) {
//                                             enermyRect[i].x+=5;
//                                         }
//                                     } else if (enermyRect[i].y % 8 != 0 && enermyRect[i].y > enermy[i].position_y * 8) {
//                                         if (single_map_1[enermy[i].position_y][enermy[i].position_x + 1] == 0 && single_map_1[enermy[i].position_y + 1][enermy[i].position_x + 1] == 0) {
//                                             enermyRect[i].x+=5;
//                                         }
//                                     } else if (enermyRect[i].y % 8 == 0) {
//                                         enermyRect[i].x+=5;
//                                     }
//                                 } else if (enermyRect[i].x % 8 == 7 && enermyRect[i].x + 1 == (enermy[i].position_x + 1) * 8) {
//                                     enermyRect[i].x+=5;
//                                     enermy[i].position_x++;
//                                 } else if (enermyRect[i].x % 8 != 7) {
//                                     enermyRect[i].x+=5;
//                                 }
//                                 break;
//                                 default:
//                                     break;
//                             }

//                         }
//                         for (int i = 0; i < 5; i++) {
//                             SDL_RenderCopy(renderer, enermyTexture[i], NULL, &enermyRect[i]);
//                         }

// ///---------------------end render enermy-----------------------------------------------------------
//                 for (int i = 0; i < totalBullets; i ++) {
//                     if (bullet[i].is_active == 1) {
//                         SDL_RenderCopy(renderer, bulletTexture[i], NULL, &bulletRect[i]);
//                     }
//                     if (bullet[i].is_active == 1) {
//                         if (bullet[i].direction == UP) {
//                             // bullet[i].position_y = bulletRect[i].y / 5;
//                             if (single_map_1[(bulletRect[i].y)/40 - 1][(bulletRect[i].x-80)/40] != 0 && bulletRect[i].y % 40 == 0) {
//                                 bullet[i].is_active = 0;
//                                 SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
//                             } else {
//                                 bulletRect[i].y -= 1;
//                             }
//                         } else if (bullet[i].direction == DOWN) {
//                             if (single_map_1[(bulletRect[i].y)/40 + 1][(bulletRect[i].x-80)/40] != 0 && bulletRect[i].y % 40 == 0) {
//                                 bullet[i].is_active = 0;
//                                 SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
//                             }
//                             bulletRect[i].y += 1;
//                             // bullet[i].position_y = bulletRect[i].y / 5;
//                         } else if (bullet[i].direction == RIGHT) {
//                             if (single_map_1[(bulletRect[i].y)/40][(bulletRect[i].x-80)/40 + 1] != 0 && bulletRect[i].x % 40 == 0) {
//                                 bullet[i].is_active = 0;
//                                 SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
//                             }
//                             bulletRect[i].x += 1;
//                             // bullet[i].position_x = bulletRect[i].x / 5;
//                         } else {
//                             if (single_map_1[(bulletRect[i].y)/40][(bulletRect[i].x-80)/40 - 1] != 0 && bulletRect[i].x % 40 == 0) {
//                                 bullet[i].is_active = 0;
//                                 SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
//                             }
//                             bulletRect[i].x -= 1;
//                             // bullet[i].position_x = bulletRect[i].x / 5;
//                             }
//                         }
//                     }
                } 
// else if (state == CHOOSE_ROOM) {
//                     SDL_RenderClear(renderer);

//                     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//                     SDL_RenderCopy(renderer, roomsBigTextTexture, NULL, &roomsBigTextRenderQuad);

//                     // render create room button
//                     Button createRoomButton;
//                     initButton(&createRoomButton, 480, 110, 80, 40, " + ");

//                     SDL_SetRenderDrawColor(renderer, 0, 0, 255, 20);
//                     SDL_Rect createRoomButtonRect = { createRoomButton.boxRect.x, createRoomButton.boxRect.y, createRoomButton.boxRect.w, createRoomButton.boxRect.h };
//                     SDL_RenderFillRect(renderer, &createRoomButtonRect);

//                     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);
//                     // Render the text on the button
//                     SDL_Color buttonTextColor = {255, 255, 255, 255};

//                     SDL_Surface *buttonTextSurface = TTF_RenderText_Solid(TINY_FONT, createRoomButton.text, buttonTextColor);
//                     SDL_Texture *buttonTextTexture = SDL_CreateTextureFromSurface(renderer, buttonTextSurface);
//                     SDL_Rect buttonTextRenderQuad = { createRoomButton.boxRect.x + 15, createRoomButton.boxRect.y + 15, buttonTextSurface->w, buttonTextSurface->h };
//                     SDL_RenderCopy(renderer, buttonTextTexture, NULL, &createRoomButtonRect);

//                     if (foundRoomState == 0) {
//                         SDL_Texture *notfoundTexture = IMG_LoadTexture(renderer, "images/nothingfound.png");
//                         SDL_Rect notfoundRenderQuad = {280, 220, 50, 50};

//                         SDL_Surface *notfoundTextSurface = TTF_RenderText_Solid(TINY_FONT, "no room found...", WHITE);
//                         SDL_Texture *notfoundTextTexture = SDL_CreateTextureFromSurface(renderer, notfoundTextSurface);
//                         SDL_Rect notfoundTextRenderQuad = { 190, 300, notfoundTextSurface->w, notfoundTextSurface->h };
//                         SDL_RenderCopy(renderer, notfoundTexture, NULL, &notfoundRenderQuad);
//                         SDL_RenderCopy(renderer, notfoundTextTexture, NULL, &notfoundTextRenderQuad);
//                     } else {
//                         SDL_Texture *roomTexture = IMG_LoadTexture(renderer, "images/room.png");
//                         renderRooms(renderer, roomTexture, SMALL_FONT);
//                     }

//                 } else if (state == PLAY_DUAL_GAME) {
//                     renderDualModeGame(renderer, myTank, friendTank, bulletTexture);

//                 } else if (state == WAITING_OTHER) {
//                     SDL_RenderClear(renderer);

//                 // Set the draw color for the gray border

//                     char in_room_sid[2];
//                     sprintf(in_room_sid, "%d", in_room_id);

//                     char title[BUFF_SIZE];
//                     strcpy(title, "ROOM ID:"); // Use strcpy to initialize the buffer
//                     strcat(title, in_room_sid);

//                     SDL_Surface *roomIdBigText = TTF_RenderText_Solid(BIG_FONT, title, WHITE);
//                     SDL_Texture *roomIdBigTextTexture = SDL_CreateTextureFromSurface(renderer, roomIdBigText);
//                     SDL_Rect roomIdBigTextRenderQuad = { 110, 100, roomIdBigText->w, roomIdBigText->h };

//                     SDL_RenderCopy(renderer, roomIdBigTextTexture, NULL, &roomIdBigTextRenderQuad);

//                     SDL_Texture *waitingTank = IMG_LoadTexture(renderer, "./images/waiting.png");
//                     SDL_Rect waitingTankRect = {245, 250, 140, 100};

//                     SDL_Surface *waitingRoomTextSurface = TTF_RenderText_Solid(TINY_FONT, "waiting for another...", WHITE);
//                     SDL_Texture *waitingRoomTextTexture = SDL_CreateTextureFromSurface(renderer, waitingRoomTextSurface);
//                     SDL_Rect waitingTextRenderQuad = { 120, 380, waitingRoomTextSurface->w, waitingRoomTextSurface->h };

//                     SDL_RenderCopy(renderer, waitingRoomTextTexture, NULL, &waitingTextRenderQuad);
//                     SDL_RenderCopy(renderer, waitingTank, NULL, &waitingTankRect);
//                 } else if (state == IN_ROOM){
//                     SDL_RenderClear(renderer);

//                 // Set the draw color for the gray border
//                     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

//                     char in_room_sid[2];
//                     sprintf(in_room_sid, "%d", in_room_id);

//                     char title[BUFF_SIZE];
//                     strcpy(title, "ROOM ID:"); // Use strcpy to initialize the buffer
//                     strcat(title, in_room_sid);

//                     SDL_Surface *roomIdBigText = TTF_RenderText_Solid(BIG_FONT, title, WHITE);
//                     SDL_Texture *roomIdBigTextTexture = SDL_CreateTextureFromSurface(renderer, roomIdBigText);
//                     SDL_Rect roomIdBigTextRenderQuad = { 110, 100, roomIdBigText->w, roomIdBigText->h };

//                     SDL_RenderCopy(renderer, roomIdBigTextTexture, NULL, &roomIdBigTextRenderQuad);

                
//                     SDL_Texture *readyTank = IMG_LoadTexture(renderer, "./images/readyTank.png");
//                     SDL_Rect readyTankRect = {245, 250, 140, 100};
                    
//                     SDL_RenderCopy(renderer, readyTank, NULL, &readyTankRect);

//                     Button startButton;
//                     initButton(&startButton, 220, 370, 200, 40, "Start");

//                     SDL_SetRenderDrawColor(renderer, 0, 0, 255, 20);
//                     SDL_Rect startButtonRect = { startButton.boxRect.x, startButton.boxRect.y, startButton.boxRect.w, startButton.boxRect.h };
//                     SDL_RenderFillRect(renderer, &startButtonRect);

//                     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);
//                     // Render the text on the button
//                     SDL_Color startButtonTextColor = {255, 255, 255, 255};

//                     SDL_Surface *startButtonTextSurface = TTF_RenderText_Solid(BIG_FONT, startButton.text, startButtonTextColor);

//                     SDL_Texture *startButtonTextTexture = SDL_CreateTextureFromSurface(renderer, startButtonTextSurface);

//                     SDL_Rect startButtonTextRenderQuad = { startButton.boxRect.x + 50, startButton.boxRect.y + 12, startButtonTextSurface->w, startButtonTextSurface->h };
//                     SDL_RenderCopy(renderer, startButtonTextTexture, NULL, &startButtonTextRenderQuad);
                        SDL_RenderPresent(renderer);
                // }

                
                while( SDL_PollEvent( &e ) ){ 
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }
                    else if (e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT) {
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
                                                // char chooseRoomMessage[BUFF_SIZE];
                                                // strcpy(chooseRoomMessage, createChooseRoomMessage());

                                                // bytes_sent = send(client_sock, chooseRoomMessage, strlen(chooseRoomMessage), 0);
                                                // if (bytes_sent < 0) {
                                                //     perror("Error sending message: ");
                                                // }

                                                // bytes_received =recv(client_sock, buff, sizeof(buff), 0);
                                                // buff[bytes_received] = '\0';

                                                // // buff[bytes_received] = '\0';
                                                // int waitingRoomNum = atoi(buff);
                                                // bytes_sent = send(client_sock, "ok", strlen("ok"), 0);

                                                // if (waitingRoomNum == 0) {
                                                //     foundRoomState = 0;
                                                // } else {
                                                //     for (int i = 1; i <= waitingRoomNum; i ++) {
                                                //         bytes_received =recv(client_sock, buff, sizeof(buff), 0);
                                                //         foundRoomState ++;
                                                //         int id, state,  first_player_id, second_player_id;
                                                //         extractAndChangeValues(buff, &id, &state, &first_player_id, &second_player_id);
                                                //         room_nums ++;
                                                //         initRoom(&rooms[room_nums], id, state, first_player_id, second_player_id);
                                                //     }
                                                // }
                                                // state = CHOOSE_ROOM;
                                            }

                                            break;
                                }

                                break;
                            case PLAY_SINGLE_GAME:
                                switch (e.key.keysym.sym) {
                                    case SDLK_DOWN:
                                        moveDown(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &controlRect, 8, 5);
                                        myTank = meDown;
                                        break;
                                    case SDLK_UP:
                                        moveUp(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &controlRect, 8, 5);
                                        myTank = meUp;
                                        break;
                                    case SDLK_RIGHT:
                                        moveRight(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &controlRect, 8, 5);
                                        myTank = meRight;
                                        break;
                                    case SDLK_LEFT:
                                        moveLeft(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &controlRect, 8 , 5);
                                        myTank = meLeft;
                                        break;
                                    case SDLK_RETURN:
                                        break;
                                    case SDLK_SPACE:
                                        // for (int i =0; i < totalBullets; i ++) {
                                        //     if (bullet[i].is_active == 0) {
                                        //         bullet[i].is_active = 1;
                                        //         if (myTank == meUp) {
                                        //              bullet[i].direction = UP;

                                        //         } else if (myTank == meDown) {
                                        //              bullet[i].direction = DOWN;
                                        //         } else if (myTank == meRight) {
                                        //              bullet[i].direction = RIGHT;
                                        //         } else {
                                        //              bullet[i].direction = LEFT;
                                        //         }
                                        //         break;
                                        //     }
                                        // }
                                       break;
                                }
                                break;
                //             case PLAY_DUAL_GAME:
                //                 switch (e.key.keysym.sym) {
                //                         case SDLK_DOWN:
                //                             positive = 1;
                //                             if (first_user == 1) {

                //                                 moveDown(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &controlRect);
                //                                 myTank = meDown;

                //                                 char directionMessage[BUFF_SIZE];
                //                                 strcpy(directionMessage, createDirectionMessage(in_room_id, myId, DOWN));
                //                                 bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                //                             } else {
                //                                 moveDown(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &friendRect);
                //                                 myTank = meDown;
                //                                  char directionMessage[BUFF_SIZE];
                //                                 strcpy(directionMessage, createDirectionMessage(in_room_id, myId, DOWN));
                //                                 bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                //                             }
                //                             break;
                //                         case SDLK_UP:
                //                             positive = 1;
                //                             if (first_user == 1) {
                //                                 moveUp(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &controlRect);
                //                                 myTank = meUp;
                //                                  char directionMessage[BUFF_SIZE];
                //                                 strcpy(directionMessage, createDirectionMessage(in_room_id, myId, UP));
                //                                 bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);

                //                             } else {
                //                                 moveUp(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &friendRect);
                //                                 myTank = meUp;
                //                                  char directionMessage[BUFF_SIZE];
                //                                 strcpy(directionMessage, createDirectionMessage(in_room_id, myId, UP));
                //                                 bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                //                             }
                //                             break;
                //                         case SDLK_RIGHT:
                //                             positive = 1;
                //                             if (first_user == 1) {
                //                                 moveRight(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &controlRect);
                //                                 myTank = meRight;
                //                                  char directionMessage[BUFF_SIZE];
                //                                 strcpy(directionMessage, createDirectionMessage(in_room_id, myId, RIGHT));
                //                                 bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                //                             } else {
                //                                 moveRight(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &friendRect);
                //                                 myTank = meRight;
                //                                  char directionMessage[BUFF_SIZE];
                //                                 strcpy(directionMessage, createDirectionMessage(in_room_id, myId, RIGHT));
                //                                 bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                //                             }
                //                             break;
                //                         case SDLK_LEFT:
                //                             positive = 1;
                //                             if (first_user == 1) {
                //                                 moveLeft(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &controlRect);
                //                                 myTank = meLeft;
                //                                  char directionMessage[BUFF_SIZE];
                //                                 strcpy(directionMessage, createDirectionMessage(in_room_id, myId, LEFT));
                //                                 bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                //                             } else {
                //                                 moveLeft(&hozirontal_controller, &vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &friendRect);
                //                                 myTank = meLeft;
                //                                  char directionMessage[BUFF_SIZE];
                //                                 strcpy(directionMessage, createDirectionMessage(in_room_id, myId, LEFT));
                //                                 bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                //                             }
                //                             break;
                //                     case SDLK_RETURN:
                //                         break;
                //                     case SDLK_SPACE:
                //                         positive = 1;
                //                         for (int i =0; i < totalBullets; i ++) {
                //                             if (bullet[i].is_active == 0) {
                //                                 bullet[i].is_active = 1;
                //                                  if (first_user == 1 && positive == 1) {
                //                                     bulletRect[i].x = controlRect.x;
                //                                     bulletRect[i].y = controlRect.y;
                //                                 } else if (first_user == 0 && positive == 1){
                //                                     bulletRect[i].x = friendRect.x;
                //                                     bulletRect[i].y = friendRect.y;
                //                                 }
                //                                 if (myTank == meUp) {
                //                                     bullet[i].direction = UP;
                //                                     char bulletDirectionMessage[BUFF_SIZE];
                //                                     strcpy(bulletDirectionMessage, createBulletDirectionMessage(in_room_id, myId, UP));
                //                                     bytes_sent = send(client_sock, bulletDirectionMessage, strlen(bulletDirectionMessage), 0);
                //                                 } else if (myTank == meDown) {
                //                                      bullet[i].direction = DOWN;
                //                                      char bulletDirectionMessage[BUFF_SIZE];
                //                                     strcpy(bulletDirectionMessage, createBulletDirectionMessage(in_room_id, myId, DOWN));
                //                                     bytes_sent = send(client_sock, bulletDirectionMessage, strlen(bulletDirectionMessage), 0);
                //                                 } else if (myTank == meRight) {
                //                                      bullet[i].direction = RIGHT;
                //                                      char bulletDirectionMessage[BUFF_SIZE];
                //                                     strcpy(bulletDirectionMessage, createBulletDirectionMessage(in_room_id, myId, RIGHT));
                //                                     bytes_sent = send(client_sock, bulletDirectionMessage, strlen(bulletDirectionMessage), 0);
                //                                 } else {
                //                                      bullet[i].direction = LEFT;
                //                                      char bulletDirectionMessage[BUFF_SIZE];
                //                                     strcpy(bulletDirectionMessage, createBulletDirectionMessage(in_room_id, myId, LEFT));
                //                                     bytes_sent = send(client_sock, bulletDirectionMessage, strlen(bulletDirectionMessage), 0);
                //                                 }
                //                                 break;
                //                             }
                //                         }
                //                        break;
                //                 }
                //                 break;
                        }
                    }
                
                    else if (e.type == SDL_MOUSEBUTTONDOWN) {
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
                        }

                        else if (state == REGISTER) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            setFocusOnTextbox(&usernameRegisterTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&passwordRegisterTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&retypePasswordRegisterTextbox, mouseX, mouseY);

                            if (mouseX > 230 && mouseX < 430 && mouseY > 380 && mouseY < 430){
                                char registerMessage[BUFF_SIZE];

                                if (strlen(usernameRegisterTextbox.text) == 1 || strlen(passwordRegisterTextbox.text) == 1 || strlen(retypePasswordRegisterTextbox.text) == 1) {
                                    strcpy(register_message, "Please input all fields");
                                } else {

                                    strcpy(registerMessage, createRegisterMessage(usernameRegisterTextbox.text, passwordRegisterTextbox.text));

                                    bytes_sent = send(client_sock, registerMessage, strlen(registerMessage), 0);
                                    if (bytes_sent < 0) {
                                        perror("Error sending message: ");
                                    }
                            
                                    bytes_received = recv(client_sock, buff, sizeof(buff), 0);    
                                    
                                    buff[bytes_received] = '\0';

                                    if (strcmp(buff, "existed") == 0 ) {
                                        strcpy(register_message, "This account existed");
                                    } else {
                                        strcpy(register_message, "Successfully registered");
                                    }
                                }
                            }
                        }
                // else if (state == CHOOSE_ROOM) {
                //             SDL_GetMouseState(&mouseX, &mouseY);
                //             if (mouseX >= 480 && mouseX <= 560 && mouseY >= 110 && mouseY <= 150) {
                //                 char message[BUFF_SIZE];

                //                 strcpy(message , createCreateRoomMessage(myId));
                //                 bytes_sent = send(client_sock, message, strlen(message), 0);
                //                 if (bytes_sent < 0) {
                //                     perror("Error sending message: ");
                //                 }
                        
                //                 bytes_received = recv(client_sock, buff, sizeof(buff), 0);    
                //                 if (bytes_received < 0) {
                //                     perror("Error: ");
                //                     close(client_sock);
                //                 }
                //                 buff[bytes_received] = '\0';
                //                 in_room_id = atoi(buff);

                //                 rooms[in_room_id].status = 1;
                //                 rooms[in_room_id].first_player_id = myId;
                //                 state = WAITING_OTHER;
                //                 first_user = 1;
                //                 dual_mode_position_x = 4;
                //                 dual_mode_position_y = 11;

                //                 dual_mode_position_x_friend = 8;
                //                 dual_mode_position_y_friend = 11;
                //                 vertical_controller_friend = 88;
                //                 hozirontal_controller_friend = 64;

                //                 if (pthread_create(&tid, NULL, receiveThread, (void*)&client_sock) != 0) {
                //                     perror("Error creating thread");
                //                     return 1;
                //                 }

                //                 receiveThreadCreated = 1;  // Set the flag to indicate that the thread is created
                //                 pthread_detach(tid);
                //             } else {
                //                 SDL_GetMouseState(&mouseX, &mouseY);
                //                 int row = (mouseY - 200) /100;
                //                 int col = (mouseX - 100) / 80;
                //                 dual_mode_position_x = 8;
                //                 dual_mode_position_y = 11;
                //                 vertical_controller = 88;
                //                 hozirontal_controller = 64;

                //                 dual_mode_position_x_friend = 4;
                //                 dual_mode_position_y_friend = 11;
                //                 vertical_controller_friend = 88;
                //                 hozirontal_controller_friend = 32;
                                
                //                 if (mouseX > 100 && mouseY > 200) {
                //                     in_room_id = 1 + row*4 + col;
                                    
                //                     if (in_room_id > 0 && in_room_id <= room_nums) {
                                        
                //                         if (rooms[in_room_id].status == 1) {
                //                             char joinRoomMessage[BUFF_SIZE];
                //                             strcpy(joinRoomMessage, createJoinRoomMessage(myId, in_room_id));

                //                             bytes_sent = send(client_sock, joinRoomMessage, strlen(joinRoomMessage), 0);
                //                             if (bytes_sent < 0) {
                //                                 perror("Error sending message: ");
                //                             }
                //                             bytes_received = recv(client_sock, buff, sizeof(buff), 0);    
                //                             if (bytes_received < 0) {
                //                                 perror("Error: ");
                //                                 close(client_sock);
                //                             }

                //                             buff[bytes_received] = '\0';

                //                             if (strcmp(buff, "join success") == 0) {
                //                                 state = IN_ROOM;
                //                                 if (pthread_create(&tid, NULL, receiveThread, (void*)&client_sock) != 0) {
                //                                     perror("Error creating thread");
                //                                     return 1;
                //                                 }

                //                                 receiveThreadCreated = 1;  // Set the flag to indicate that the thread is created
                //                                 pthread_detach(tid);
                //                                 ready_to_start = 1;
                //                             }
                //                         }
                //                     }
                //                 }

                                
                //             }
                //         } else if (state == IN_ROOM && ready_to_start == 1) {
                //             SDL_GetMouseState(&mouseX, &mouseY);

                //             if (mouseX > 220 && mouseX < 420 && mouseY > 370 && mouseY < 410) {
                //                 myTank = meUp;
                //                 state = PLAY_DUAL_GAME;

                //                 char startGameMessage[BUFF_SIZE];
                //                 strcpy(startGameMessage, createStartGameMessage(in_room_id));

                //                 bytes_sent = send(client_sock, startGameMessage, strlen(startGameMessage), 0);
                //                 if (bytes_sent < 0) {
                //                     perror("Error sending message: ");
                //                 }
                //             }
                            }
                        }
                    }
            // }
        }
    }
 
    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

	close(client_sock);
	return 0;
}

// void extractAndChangeValues(const char *input, int *id, int *state, int *first_player_id, int *second_player_id) {
//     if (input != NULL && id != NULL && state != NULL && first_player_id != NULL && second_player_id != NULL) {
//         // Use sscanf to extract all four values from the input string
//         int parsed = sscanf(input, "%d*%d*%d*%d", id, state, first_player_id, second_player_id);

//         if (parsed != 4) {
//             // Handle the case where parsing was unsuccessful (e.g., invalid input)
//             // You can set default values or handle errors here as needed.
//             *id = -1;
//             *state = -1;
//             *first_player_id = -1;
//             *second_player_id = -1;
//         } 
//     }
// }


// void renderRooms(SDL_Renderer *renderer, SDL_Texture *roomTexture, TTF_Font *font) {
//     int roomWidth = 100;
//     int roomHeight = 80;
//     int spacingX = 20; // Adjust the horizontal spacing between rooms
//     int spacingY = 20; // Adjust the vertical spacing between rooms

//     for (int i = 1; i <= room_nums; i++) {
//         int row = (i - 1) / 4;
//         int col = (i - 1) % 4;

//         SDL_Rect roomRect = {
//             100 + col * (roomWidth + spacingX),
//             200 + row * (roomHeight + spacingY),
//             roomWidth,
//             roomHeight
//         };

//         SDL_RenderCopy(renderer, roomTexture, NULL, &roomRect);

//         // Render room id in the top left corner
//         char roomIdText[2]; // Adjust the size as needed
//         snprintf(roomIdText, sizeof(roomIdText), "%d", rooms[i].id);

//         SDL_Surface *textSurface = TTF_RenderText_Solid(font, roomIdText, (SDL_Color){0, 0, 0, 255});
//         SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
//         SDL_FreeSurface(textSurface);

//         int textWidth, textHeight;
//         SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

//         SDL_Rect textRect = {
//             roomRect.x,
//             roomRect.y,
//             textWidth,
//             textHeight
//         };

//         SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

//         char roomStatusText[2]; // Adjust the size as needed
//         snprintf(roomStatusText, sizeof(roomIdText), "%d", rooms[i].status);

//         SDL_Surface *roomStatusTextSurface = TTF_RenderText_Solid(TINY_FONT, roomStatusText, (SDL_Color){0, 0, 0, 255});
//         SDL_Texture *roomStatusTextTexture = SDL_CreateTextureFromSurface(renderer, roomStatusTextSurface);
//         SDL_FreeSurface(roomStatusTextSurface);

//         SDL_Rect roomStatusRect = {
//             roomRect.x+18,
//             roomRect.y+25,
//             roomStatusTextSurface->w,
//             roomStatusTextSurface->h
//         };

//         SDL_RenderCopy(renderer, roomStatusTextTexture, NULL, &roomStatusRect);
//         SDL_Texture *personTexture = IMG_LoadTexture(renderer, "images/person.jpg");

//         SDL_Rect personRect = {
//             roomRect.x+35,
//             roomRect.y+25,
//             15,15
//         };

//         SDL_RenderCopy(renderer, personTexture, NULL, &personRect);
//     }
// }

// void* receiveThread(void* arg) {
//     int client_sock = *((int*)arg);
//     char buff[BUFF_SIZE];
//     int bytes_received;

//     while (1) {
//         // Receive messages from the server
//         bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);
//         if (bytes_received <= 0) {
//             printf("\nConnection closed!\n");
//             break;
//         }
//         buff[bytes_received] = '\0';
//         // Print received messages only when the chat has started
//         if (strcmp(buff, "other joined") == 0) {
//             ready_to_start = 1;
//             state = IN_ROOM;
//         } else if (strcmp(buff, "start game") == 0) {
//             state = PLAY_DUAL_GAME;
//         } else if (buff[0] == 0x07) {
//             positive = 0;
//             int direction;
//             sscanf(buff + 1, "%d", &direction); // Skip the first byte (0x05) and read the integer.

//             if (first_user == 1) {
                
//                 if (direction == UP) {
//                     friendTank = meUp;
//                     moveUp(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &friendRect);
//                 } else if (direction == DOWN) {
//                     friendTank = meDown;
//                     moveDown(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &friendRect);
//                 } else if (direction == RIGHT) {
//                     friendTank = meRight;
//                     moveRight(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &friendRect);
//                 } else {
//                     friendTank = meLeft;
//                     moveLeft(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &friendRect);
//                 }
//             } else {
//                 if (direction == UP) {
//                     friendTank = meUp;
//                     moveUp(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &controlRect);
//                 } else if (direction == DOWN) {
//                     friendTank = meDown;
//                     moveDown(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &controlRect);
//                 } else if (direction == RIGHT) {
//                     friendTank = meRight;
//                     moveRight(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &controlRect);
//                 } else {
//                     friendTank = meLeft;
//                     moveLeft(&hozirontal_controller_friend, &vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &controlRect);
//                 }
//             }
//         } else if (buff[0] == 0x09) {
//             positive = 0;
//             int direction;

//             sscanf(buff + 1, "%d", &direction); // Skip the first byte (0x05) and read the integer.
//            for (int i =0; i < totalBullets; i ++) {
//                 if (bullet[i].is_active == 0) {
//                     bullet[i].is_active = 1;
                    
//                     if (first_user == 1 && positive == 0) {
//                         bulletRect[i].x = friendRect.x;
//                         bulletRect[i].y = friendRect.y;
//                     } else {
//                         bulletRect[i].x = controlRect.x;
//                         bulletRect[i].y = controlRect.y;
//                     }
//                     if (direction == UP) {
//                             bullet[i].direction = UP;
//                     } else if (direction == DOWN) {
//                             bullet[i].direction = DOWN;
//                     } else if (direction == RIGHT) {
//                             bullet[i].direction = RIGHT;
//                     } else {
//                             bullet[i].direction = LEFT;
//                     }
//                     break;
//                 }
//             }
//         }   
//     }

//     pthread_exit(NULL);
// }

// void renderDualModeGame(SDL_Renderer *renderer, SDL_Texture *myTank, SDL_Texture *friendTank, SDL_Texture* bulletTexture[]) {

//     SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
//     SDL_RenderClear(renderer);

//     SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//     // Calculate the map boundaries
//     int mapX = (640 - (12 * 40)) / 2;
//     int mapY = (480 - (12 * 40)) / 2;

//     // Render the gray border around the map
//     SDL_Rect borderRect = { mapX - 2, mapY - 2, 12 * 40 + 4, 12 * 40 + 4 };
//     SDL_RenderFillRect(renderer, &borderRect);

//     // Render the map
//     for (int y = 0; y < 12; y++) {
//         for (int x = 0; x < 12; x++) {
//             SDL_Rect destRect = { mapX + x * 40, mapY + y * 40, 40, 40 };
//             SDL_Texture* texture = NULL;

//             switch (dual_map_1[y][x]) {
//                 case 0:
//                     break;
//                 case 1:
//                     texture = block;
//                     break;
//                 case 2:
//                     texture = iron;
//                     break;
//                 case 9:
//                     if (first_user == 1) {
//                         texture = myTank;
//                         SDL_RenderCopy(renderer, texture, NULL, &controlRect);
//                     } else {
//                         texture = friendTank;
//                         SDL_RenderCopy(renderer, texture, NULL, &controlRect);
//                     }
//                     break;
//                 case 8:
//                     if (first_user == 0) {
//                         texture = myTank;
//                         SDL_RenderCopy(renderer, texture, NULL, &friendRect);
//                     } else {
//                         texture = friendTank;
//                         SDL_RenderCopy(renderer, texture, NULL, &friendRect);
//                     }
//                     break;
//                 case 7:
//                     texture = enermy_1_right;
//                     break;
//                 default:
//                     break;
//             }

//             if (texture != myTank) { 
//                 SDL_RenderCopy(renderer, texture, NULL, &destRect);
//             }
//         }
//     }

//      for (int i = 0; i < totalBullets; i ++) {
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
