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
#include <signal.h>
#include <math.h>
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

int foundRoomState = 1;
int room_nums = 0;
int readyToStart = 0;

int positive = 1;
SDL_Renderer *renderer;

Room rooms[50];

void initRoom(Room *room, int id, int status, int first_player_id, int second_player_id) {
    room->id = id;
    room->status = status;
    room->first_player_id = first_player_id;
    room->second_player_id = second_player_id;
}

void setFocusOnTextbox(Textbox *textbox, int mouseX, int mouseY);
void moveDown(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *single_controlRect, int stepEachBlock, int stepSize, int single_map[12][12]);
void moveUp(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *single_controlRect, int stepEachBlock, int stepSize, int single_map[12][12]);
void moveRight(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *single_controlRect, int stepEachBlock, int stepSize, int single_map[12][12]);
void moveLeft(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *single_controlRect, int stepEachBlock, int stepSize, int single_map[12][12]);
void extractAndChangeValues(const char *input, int *id, int *state, int *first_player_id, int *second_player_id);
void renderRooms(SDL_Renderer *renderer, SDL_Texture *roomTexture, TTF_Font *font);
void checkRooms();
void* receiveThread(void* arg);
void* receiveThread1(void* arg);
void renderDualModeGame(SDL_Renderer *renderer, SDL_Texture *myTank, SDL_Texture *friendTank, SDL_Texture* bulletTexture[]);

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
    pthread_t pid;

    int receiveThreadCreated1 = 0;
    int receiveThreadCreated2 = 0;
    
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
            SDL_Rect rankingBigTextRenderQuad = { 195, 100, rankingBigTextSurface->w, rankingBigTextSurface->h };
            SDL_Rect gameOverBigTextRenderQuad = { 150, 250, gameOverBigTextSurface->w, gameOverBigTextSurface->h };
            SDL_Rect roomsBigTextRenderQuad = { 195, 100, roomsBigText->w, roomsBigText->h };
            SDL_Rect usernameRenderQuad = { 120, 200, usernameText->w, usernameText->h };
            SDL_Rect passwordRenderQuad = { 120, 250, passwordText->w, passwordText->h };
            SDL_Rect retypePasswordRenderQuad = { 120, 300, retypePasswordText->w, retypePasswordText->h };
            SDL_Rect chooseModeRenderQuad = { 50, 100, chooseModeText->w, chooseModeText->h };
            SDL_Rect rankingRenderQuad = { 600, 450, 80, 80};
            SDL_Rect singleModeRenderQuad = { 250, 250, singleModeText->w, singleModeText->h };
            SDL_Rect dualModeRenderQuad = { 250, 300, dualModeText->w, dualModeText->h };
            SDL_Rect tankRectChooseMode = {200, 242, TANK_WIDTH, TANK_HEIGHT};
            SDL_Rect leaveRenderQuad = {30, 100, 40, 40};


            initTextbox(&usernameLoginTextbox, 320, 195, 200, 30);
            initTextbox(&passwordLoginTextbox, 320, 245, 200, 30);
            
            initTextbox(&usernameRegisterTextbox, 300, 195, 200, 30);
            initTextbox(&passwordRegisterTextbox, 300, 245, 200, 30);
            initTextbox(&retypePasswordRegisterTextbox, 410, 295, 200, 30);
            SDL_Rect showPasswordRenderQuad = {passwordRegisterTextbox.boxRect.x + 300, passwordRegisterTextbox.boxRect.y-2, 28, 28};
            SDL_Rect showRetypedPasswordRenderQuad = {passwordRegisterTextbox.boxRect.x + 300, passwordRegisterTextbox.boxRect.y+60, 28, 28};

            initValueForSingle1();
            initValueForSingle2();
            initValueForSingle3();

            initValueForDual();
            initValueForDual_friend();

            while( quit == false ){
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);  // Black background
                    SDL_RenderClear(renderer);

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


                    /////
                    void renderPassword(SDL_Renderer *renderer, TTF_Font *font, const char *password, SDL_Rect renderQuad, int showPassword) {
                        SDL_Surface *passwordTextSurface;
                        if (showPassword) {
                            // hien thi mat khau 
                            passwordTextSurface = TTF_RenderText_Solid(font, password, WHITE);
                        } else {
                            // hien thi mat khau duoi dang *
                            size_t passwordLength = strlen(password);
                            char obscuredPassword[passwordLength ];
                            for (size_t i = 1; i < passwordLength; ++i) {
                                obscuredPassword[i] = '*';
                            }
                            obscuredPassword[0] = ' ';
                            obscuredPassword[passwordLength] = '\0';
                            passwordTextSurface = TTF_RenderText_Solid(font, obscuredPassword, WHITE);
                        }

                        SDL_Texture *passwordTextTexture = SDL_CreateTextureFromSurface(renderer, passwordTextSurface);
                        SDL_RenderCopy(renderer, passwordTextTexture, NULL, &renderQuad);

                        SDL_FreeSurface(passwordTextSurface);
                        SDL_DestroyTexture(passwordTextTexture);
                    }
                    /////


                    SDL_Surface *passwordBoxTextSurface = TTF_RenderText_Solid(TINY_FONT, passwordLoginTextbox.text, WHITE);
                    SDL_Texture *passwordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, passwordBoxTextSurface);
                    int passwordTextWidth = passwordBoxTextSurface->w;
                    int passwordTextHeight = passwordBoxTextSurface->h;
                    SDL_Rect passwordBoxRenderQuad = { passwordLoginTextbox.boxRect.x + 5, passwordLoginTextbox.boxRect.y + 5, passwordTextWidth, passwordTextHeight };
                    renderPassword(renderer, TINY_FONT, passwordLoginTextbox.text, passwordBoxRenderQuad, showPasswordLogin);

                    SDL_RenderCopy(renderer, usernameBoxTextTexture, NULL, &usernameBoxRenderQuad);
                    SDL_RenderCopy(renderer, hidePasswordTexture, NULL, &showPasswordRenderQuad);

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
                    
                    ////////////////////////////
                    
                    void renderPassword(SDL_Renderer *renderer, TTF_Font *font, const char *password, SDL_Rect renderQuad, int showPassword) {
                        SDL_Surface *passwordTextSurface;
                        if (showPassword) {
                            // hien thi mat khau 
                            passwordTextSurface = TTF_RenderText_Solid(font, password, WHITE);
                        } else {
                            // hien thi mat khau duoi dang *
                            size_t passwordLength = strlen(password);
                            char obscuredPassword[passwordLength ];
                            for (size_t i = 1; i < passwordLength; ++i) {
                                obscuredPassword[i] = '*';
                            }
                            obscuredPassword[0] = ' ';
                            obscuredPassword[passwordLength] = '\0';
                            passwordTextSurface = TTF_RenderText_Solid(font, obscuredPassword, WHITE);
                        }

                        SDL_Texture *passwordTextTexture = SDL_CreateTextureFromSurface(renderer, passwordTextSurface);
                        SDL_RenderCopy(renderer, passwordTextTexture, NULL, &renderQuad);

                        SDL_FreeSurface(passwordTextSurface);
                        SDL_DestroyTexture(passwordTextTexture);
                    }

                    /////////////////////////////
                    

                    /////////////////////

                    SDL_Surface *passwordBoxTextSurface = TTF_RenderText_Solid(TINY_FONT, passwordRegisterTextbox.text, WHITE);
                    SDL_Texture *passwordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, passwordBoxTextSurface);

                    int passwordTextWidth = passwordBoxTextSurface->w;
                    int passwordTextHeight = passwordBoxTextSurface->h;
                    SDL_Rect passwordBoxRenderQuad = { passwordRegisterTextbox.boxRect.x + 5, passwordRegisterTextbox.boxRect.y + 5, passwordTextWidth, passwordTextHeight };
                    renderPassword(renderer, TINY_FONT, passwordRegisterTextbox.text, passwordBoxRenderQuad, showPassword);
        

                    ////////////////////

                    SDL_Surface *retypePasswordRegisterBoxTextSurface = TTF_RenderText_Solid(TINY_FONT, retypePasswordRegisterTextbox.text, WHITE);
                    SDL_Texture *retypePasswordBoxTextTexture = SDL_CreateTextureFromSurface(renderer, retypePasswordRegisterBoxTextSurface);

                    int retypePasswordTextWidth = retypePasswordRegisterBoxTextSurface->w;
                    int retypePasswordTextHeight = retypePasswordRegisterBoxTextSurface->h;
                    SDL_Rect retypePasswordBoxRenderQuad = { retypePasswordRegisterTextbox.boxRect.x + 5, retypePasswordRegisterTextbox.boxRect.y + 5, retypePasswordTextWidth, retypePasswordTextHeight };
                    renderPassword(renderer, TINY_FONT, retypePasswordRegisterTextbox.text, retypePasswordBoxRenderQuad, showRetypePassword);
                    ///////



                    SDL_RenderCopy(renderer, usernameBoxTextTexture, NULL, &usernameBoxRenderQuad);
                    //SDL_RenderCopy(renderer, passwordBoxTextTexture, NULL, &passwordBoxRenderQuad);
                    //SDL_RenderCopy(renderer, retypePasswordBoxTextTexture, NULL, &retypePasswordBoxRenderQuad);

                  
                    SDL_RenderCopy(renderer, hidePasswordTexture, NULL, &showPasswordRenderQuad);
                    SDL_RenderCopy(renderer, hideRetypePasswordTexture, NULL, &showRetypedPasswordRenderQuad);
                    

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
                    SDL_RenderCopy(renderer, rankingTexture, NULL, &rankingRenderQuad);
                    SDL_RenderCopy(renderer, singleModeTextTexture, NULL, &singleModeRenderQuad);
                    SDL_RenderCopy(renderer, dualModeTextTexture, NULL, &dualModeRenderQuad);
                    SDL_RenderCopy(renderer, tank, NULL, &tankRectChooseMode);

                    if (scoreUpdated == 0) {
                        initValueForSingle1();
                        initValueForSingle2();
                        initValueForSingle3();

                        initValueForDual();
                        initValueForDual_friend();
                        char updateScoreMessage[BUFF_SIZE];
                        strcpy(updateScoreMessage, createUpdateScoreMessage(myId, single_scores));

                        bytes_sent = send(client_sock, updateScoreMessage, strlen(updateScoreMessage), 0);
                        scoreUpdated = 1;
                    }
                }
                else if (state == PLAY_SINGLE_MAP_1) {
                scoreUpdated = 0;
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                renderSingle(renderer, single_map_1, 1);

                renderSingle1Enermies(renderer, single_1_enermies);

                renderBulletSingle1(renderer);
                SDL_Surface *scoreTextSurface = TTF_RenderText_Solid(TINY_FONT, "SCORE:", BLACK);
                SDL_Texture *scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreTextSurface);
                SDL_Rect scoreTextRenderQuad = { 580, 250, scoreTextSurface->w, scoreTextSurface->h };
                SDL_RenderCopy(renderer, scoreTextTexture, NULL, &scoreTextRenderQuad);

                char sscores[BUFF_SIZE];
                sprintf(sscores, "%d", single_scores);

                SDL_Surface *scoreNumTextSurface = TTF_RenderText_Solid(TINY_FONT, sscores, BLACK);
                SDL_Texture *scoreNumTextTexture = SDL_CreateTextureFromSurface(renderer, scoreNumTextSurface);
                SDL_Rect scoreNumTextRenderQuad = { 610, 290, scoreNumTextSurface->w, scoreNumTextSurface->h };
                SDL_RenderCopy(renderer, scoreNumTextTexture, NULL, &scoreNumTextRenderQuad);

            } else if (state == CHANGING_TO_SINGLE_MAP_2) {
                SDL_RenderClear(renderer);
                renderChangeMapScreen(renderer, 2);
            } else if (state == PLAY_SINGLE_MAP_2) {
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                renderSingle(renderer, single_map_2, 2);

                SDL_Surface *scoreTextSurface = TTF_RenderText_Solid(TINY_FONT, "SCORE:", BLACK);
                SDL_Texture *scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreTextSurface);
                SDL_Rect scoreTextRenderQuad = { 580, 250, scoreTextSurface->w, scoreTextSurface->h };
                SDL_RenderCopy(renderer, scoreTextTexture, NULL, &scoreTextRenderQuad);

                char sscores[BUFF_SIZE];
                sprintf(sscores, "%d", single_scores);

                SDL_Surface *scoreNumTextSurface = TTF_RenderText_Solid(TINY_FONT, sscores, BLACK);
                SDL_Texture *scoreNumTextTexture = SDL_CreateTextureFromSurface(renderer, scoreNumTextSurface);
                SDL_Rect scoreNumTextRenderQuad = { 610, 290, scoreNumTextSurface->w, scoreNumTextSurface->h };
                SDL_RenderCopy(renderer, scoreNumTextTexture, NULL, &scoreNumTextRenderQuad);

                renderSingle2Enermies(renderer, single_2_enermies);

                renderBulletSingle2(renderer);

            } else if (state == CHANGING_TO_SINGLE_MAP_3) {
                SDL_RenderClear(renderer);
                renderChangeMapScreen(renderer, 3);

            } else if (state == PLAY_SINGLE_MAP_3) {

                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                renderSingle(renderer, single_map_3, 3);

                SDL_Surface *scoreTextSurface = TTF_RenderText_Solid(TINY_FONT, "SCORE:", BLACK);
                SDL_Texture *scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreTextSurface);
                SDL_Rect scoreTextRenderQuad = { 580, 250, scoreTextSurface->w, scoreTextSurface->h };
                SDL_RenderCopy(renderer, scoreTextTexture, NULL, &scoreTextRenderQuad);

                char sscores[BUFF_SIZE];
                sprintf(sscores, "%d", single_scores);

                SDL_Surface *scoreNumTextSurface = TTF_RenderText_Solid(TINY_FONT, sscores, BLACK);
                SDL_Texture *scoreNumTextTexture = SDL_CreateTextureFromSurface(renderer, scoreNumTextSurface);
                SDL_Rect scoreNumTextRenderQuad = { 610, 290, scoreNumTextSurface->w, scoreNumTextSurface->h };
                SDL_RenderCopy(renderer, scoreNumTextTexture, NULL, &scoreNumTextRenderQuad);

                renderSingle3Enermies(renderer, single_3_enermies);

                renderBulletSingle3(renderer);
            }
            else if (state == CHOOSE_ROOM) {
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderCopy(renderer, roomsBigTextTexture, NULL, &roomsBigTextRenderQuad);

                SDL_Texture *createRoomButton = IMG_LoadTexture(renderer, "./images/plus.png");
                SDL_Rect createRoomRenderQuad = {480, 110, 40, 40};

                SDL_RenderCopy(renderer, createRoomButton, NULL, &createRoomRenderQuad);

                if (foundRoomState == 0) {
                    SDL_Texture *notfoundTexture = IMG_LoadTexture(renderer, "images/nothingfound.png");
                    SDL_Rect notfoundRenderQuad = {280, 220, 50, 50};

                    SDL_Surface *notfoundTextSurface = TTF_RenderText_Solid(TINY_FONT, "no room found...", WHITE);
                    SDL_Texture *notfoundTextTexture = SDL_CreateTextureFromSurface(renderer, notfoundTextSurface);
                    SDL_Rect notfoundTextRenderQuad = { 190, 300, notfoundTextSurface->w, notfoundTextSurface->h };
                    SDL_RenderCopy(renderer, notfoundTexture, NULL, &notfoundRenderQuad);
                    SDL_RenderCopy(renderer, notfoundTextTexture, NULL, &notfoundTextRenderQuad);
                } else {
                    SDL_Texture *roomTexture = IMG_LoadTexture(renderer, "images/room.png");
                    renderRooms(renderer, roomTexture, SMALL_FONT);
                }
            } 
            else if (state == PLAY_DUAL_GAME) {
                scoreUpdated = 0;
                // renderDualModeGame(renderer, myTank, friendTank, bulletTexture);
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
                SDL_RenderClear(renderer);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                renderDual(renderer, dual_map_1, 1);
                if (isFirstUserInRoom == 1) {
                    renderBulletDual(renderer);
                    renderBulletDual_friend_positive(renderer);
                } else {
                    renderBulletDual_friend(renderer);
                    renderBulletDual_postive(renderer);
                }

                SDL_Surface *scoreTextSurface = TTF_RenderText_Solid(TINY_FONT, "SCORE:", BLACK);
                SDL_Texture *scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreTextSurface);
                SDL_Rect scoreTextRenderQuad = { 580, 250, scoreTextSurface->w, scoreTextSurface->h };
                SDL_RenderCopy(renderer, scoreTextTexture, NULL, &scoreTextRenderQuad);

                char sscores[BUFF_SIZE];
                sprintf(sscores, "%d", single_scores);

                SDL_Surface *scoreNumTextSurface = TTF_RenderText_Solid(TINY_FONT, sscores, BLACK);
                SDL_Texture *scoreNumTextTexture = SDL_CreateTextureFromSurface(renderer, scoreNumTextSurface);
                SDL_Rect scoreNumTextRenderQuad = { 610, 290, scoreNumTextSurface->w, scoreNumTextSurface->h };
                SDL_RenderCopy(renderer, scoreNumTextTexture, NULL, &scoreNumTextRenderQuad);

            } 
            else if (state == WAITING_OTHER) {
                SDL_RenderClear(renderer);

                char in_room_sid[2];
                sprintf(in_room_sid, "%d", in_room_id);

                char title[BUFF_SIZE];
                strcpy(title, "ROOM ID:"); // Use strcpy to initialize the buffer
                strcat(title, in_room_sid);

                SDL_Surface *roomIdBigText = TTF_RenderText_Solid(BIG_FONT, title, WHITE);
                SDL_Texture *roomIdBigTextTexture = SDL_CreateTextureFromSurface(renderer, roomIdBigText);
                SDL_Rect roomIdBigTextRenderQuad = { 110, 100, roomIdBigText->w, roomIdBigText->h };

                SDL_RenderCopy(renderer, roomIdBigTextTexture, NULL, &roomIdBigTextRenderQuad);

                SDL_Texture *waitingTank = IMG_LoadTexture(renderer, "./images/waiting.png");
                SDL_Rect waitingTankRect = {245, 250, 140, 100};

                SDL_Surface *waitingRoomTextSurface = TTF_RenderText_Solid(TINY_FONT, "waiting for another...", WHITE);
                SDL_Texture *waitingRoomTextTexture = SDL_CreateTextureFromSurface(renderer, waitingRoomTextSurface);
                SDL_Rect waitingTextRenderQuad = { 120, 380, waitingRoomTextSurface->w, waitingRoomTextSurface->h };

                SDL_RenderCopy(renderer, waitingRoomTextTexture, NULL, &waitingTextRenderQuad);
                SDL_RenderCopy(renderer, waitingTank, NULL, &waitingTankRect);
                SDL_RenderCopy(renderer, leaveTexture, NULL, &leaveRenderQuad);
            }
            else if (state == READY_TO_PLAY_DUAL){
                    SDL_RenderClear(renderer);

                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                    char in_room_sid[2];
                    sprintf(in_room_sid, "%d", in_room_id);

                    char title[BUFF_SIZE];
                    strcpy(title, "ROOM ID:"); // Use strcpy to initialize the buffer
                    strcat(title, in_room_sid);

                    SDL_Surface *roomIdBigText = TTF_RenderText_Solid(BIG_FONT, title, WHITE);
                    SDL_Texture *roomIdBigTextTexture = SDL_CreateTextureFromSurface(renderer, roomIdBigText);
                    SDL_Rect roomIdBigTextRenderQuad = { 110, 100, roomIdBigText->w, roomIdBigText->h };

                    SDL_RenderCopy(renderer, roomIdBigTextTexture, NULL, &roomIdBigTextRenderQuad);

                
                    SDL_Texture *readyTank = IMG_LoadTexture(renderer, "./images/readyTank.png");
                    SDL_Rect readyTankRect = {245, 210, 140, 100};
                    
                    SDL_RenderCopy(renderer, readyTank, NULL, &readyTankRect);

                    SDL_Texture *startTexture = IMG_LoadTexture(renderer, "./images/start.png");
                    SDL_Rect startRenderQuad = {245, 300, 140, 100};

                    SDL_RenderCopy(renderer, startTexture, NULL, &startRenderQuad);
            } else if (state == SEEING_RANK) {
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderCopy(renderer, rankingBigTextTexture, NULL, &rankingBigTextRenderQuad);
                
                // if (strlen(topPlayer[0].username) > 0) {

                // for (int i = 0; i < 3; i ++) {
                    // SDL_Surface *usernameSurface = TTF_RenderText_Solid(TINY_FONT, topPlayer[i].username, WHITE);
                    // SDL_Texture *usernameTexture = SDL_CreateTextureFromSurface(renderer, usernameSurface);
                    // SDL_Rect usernameRenderQuad = {280, 300 + (i+1) * 40, usernameSurface->w, usernameSurface->h};

                    // SDL_RenderCopy(renderer, usernameTexture, NULL, &usernameRenderQuad);
                    // for ()
                // }
                // }
            } else if (state == GAME_OVER) {
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderCopy(renderer, gameoverBigTextTexture, NULL, &gameOverBigTextRenderQuad);
            }

            SDL_RenderPresent(renderer);

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
                                                state = PLAY_SINGLE_MAP_1;
                                            } else {
                                                char chooseRoomMessage[BUFF_SIZE];

                                                strcpy(chooseRoomMessage, createChooseRoomMessage()); // 0x04

                                                bytes_sent = send(client_sock, chooseRoomMessage, strlen(chooseRoomMessage), 0);

                                                bytes_received =recv(client_sock, buff, sizeof(buff), 0);
                                                buff[bytes_received] = '\0';

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
                            case PLAY_SINGLE_MAP_1:
                                switch (e.key.keysym.sym) {
                                    case SDLK_DOWN:
                                        moveDown(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &single_controlRect, 8, 5, single_map_1);
                                        myTank = meDown;
                                        break;
                                    case SDLK_UP:
                                        moveUp(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &single_controlRect, 8, 5, single_map_1);
                                        myTank = meUp;
                                        break;
                                    case SDLK_RIGHT:
                                        moveRight(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &single_controlRect, 8, 5, single_map_1);
                                        myTank = meRight;
                                        break;
                                    case SDLK_LEFT:
                                        moveLeft(&hozirontal_controller, &vertical_controller, &single_mode_postion_x, &single_mode_postion_y, &single_controlRect, 8 , 5, single_map_1);
                                        myTank = meLeft;
                                        break;
                                    case SDLK_SPACE:
                                        break;
                                    case SDLK_RETURN:
                                        singleShot(myTank, single_controlRect, meUp, meDown, meRight, meLeft, 1);
                                        break;
                                }
                            break;
                            case CHANGING_TO_SINGLE_MAP_2:
                                switch (e.key.keysym.sym) {
                                    case SDLK_RETURN:
                                        state = PLAY_SINGLE_MAP_2;
                                    break;
                                }
                            break;
                                case PLAY_SINGLE_MAP_2:
                                    switch (e.key.keysym.sym) {
                                        case SDLK_DOWN:
                                            moveDown(&hozirontal_controller_2, &vertical_controller_2, &single_mode_postion_x_2, &single_mode_postion_y_2, &single_controlRect_2, 8, 5, single_map_2);
                                            myTank = meDown;
                                            break;
                                        case SDLK_UP:
                                            moveUp(&hozirontal_controller_2, &vertical_controller_2, &single_mode_postion_x_2, &single_mode_postion_y_2, &single_controlRect_2, 8, 5, single_map_2);
                                            myTank = meUp;
                                            break;
                                        case SDLK_RIGHT:
                                            moveRight(&hozirontal_controller_2, &vertical_controller_2, &single_mode_postion_x_2, &single_mode_postion_y_2, &single_controlRect_2, 8, 5, single_map_2);
                                            myTank = meRight;
                                            break;
                                        case SDLK_LEFT:
                                            moveLeft(&hozirontal_controller_2, &vertical_controller_2, &single_mode_postion_x_2, &single_mode_postion_y_2, &single_controlRect_2, 8 , 5, single_map_2);
                                            myTank = meLeft;
                                            break;
                                        case SDLK_RETURN:
                                            singleShot2(myTank, single_controlRect_2, meUp, meDown, meRight, meLeft, 1);
                                            break;
                                        case SDLK_SPACE:
                                            break;
                                    }
                                break;
                                case CHANGING_TO_SINGLE_MAP_3:
                                    switch (e.key.keysym.sym) {
                                        case SDLK_RETURN:
                                            state = PLAY_SINGLE_MAP_3;
                                        break;
                                    }
                                break;
                                case PLAY_SINGLE_MAP_3:
                                    switch (e.key.keysym.sym) {
                                        case SDLK_DOWN:
                                            moveDown(&hozirontal_controller_3, &vertical_controller_3, &single_mode_postion_x_3, &single_mode_postion_y_3, &single_controlRect_3, 8, 5, single_map_3);
                                            myTank = meDown;
                                            break;
                                        case SDLK_UP:
                                            moveUp(&hozirontal_controller_3, &vertical_controller_3, &single_mode_postion_x_3, &single_mode_postion_y_3, &single_controlRect_3, 8, 5, single_map_3);
                                            myTank = meUp;
                                            break;
                                        case SDLK_RIGHT:
                                            moveRight(&hozirontal_controller_3, &vertical_controller_3, &single_mode_postion_x_3, &single_mode_postion_y_3, &single_controlRect_3, 8, 5, single_map_3);
                                            myTank = meRight;
                                            break;
                                        case SDLK_LEFT:
                                            moveLeft(&hozirontal_controller_3, &vertical_controller_3, &single_mode_postion_x_3, &single_mode_postion_y_3, &single_controlRect_3, 8 , 5, single_map_3);
                                            myTank = meLeft;
                                            break;
                                        case SDLK_RETURN:
                                            singleShot3(myTank, single_controlRect_3, meUp, meDown, meRight, meLeft, 1);
                                            break;
                                        case SDLK_SPACE:
                                            break;
                                    }
                                break;
                                case PLAY_DUAL_GAME:
                                    switch (e.key.keysym.sym) {
                                        case SDLK_DOWN:
                //                             positive = 1;
                                            if (isFirstUserInRoom == 1) {
                                                dualMoveDown(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                                                myTank = meDown;
                                            } else {
                                                dualMoveDown(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                                                friendTank = friendDown;
                                            }

                                            char directionMessage[BUFF_SIZE];

                                            strcpy(directionMessage, createDirectionMessage(in_room_id, myId, DOWN));
                                            
                                            bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                        break;
                                        case SDLK_UP:
                //                             positive = 1;
                                            if (isFirstUserInRoom == 1) {
                                                dualMoveUp(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                                                myTank = meUp;
                                            } else {
                                                dualMoveUp(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                                                friendTank = friendUp;
                                            }

                                            strcpy(directionMessage, createDirectionMessage(in_room_id, myId, UP));
                                            
                                            bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                        break;
                                        case SDLK_RIGHT:
                                            if (isFirstUserInRoom == 1) {
                                                dualMoveRight(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                                                myTank = meRight;
                                            } else {
                                                dualMoveRight(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                                                friendTank = friendRight;
                                            }
                                            
                                            strcpy(directionMessage, createDirectionMessage(in_room_id, myId, RIGHT));
                                            
                                            bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);
                                        break;
                                        case SDLK_LEFT:
                                            if (isFirstUserInRoom == 1) {
                                                dualMoveLeft(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                                                myTank = meLeft;
                                            } else {
                                                dualMoveLeft(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                                                friendTank = friendLeft;
                                            }
                                           
                                            strcpy(directionMessage, createDirectionMessage(in_room_id, myId, LEFT));
                                            
                                            bytes_sent = send(client_sock, directionMessage, strlen(directionMessage), 0);

                                        break;
                //                     case SDLK_RETURN:
                //                         break;
                                        case SDLK_RETURN:
                                            if (isFirstUserInRoom == 1) {
                                                dualShot(myTank, dual_controlRect, meUp, meDown, meRight, meLeft, 1);

                                                char dualShotMessage[BUFF_SIZE];
                                                if (myTank == meUp) {
                                                    strcpy(dualShotMessage, createDualShotMessage(in_room_id, myId, UP));
                                                } else if (myTank == meDown) {
                                                    strcpy(dualShotMessage, createDualShotMessage(in_room_id, myId, DOWN));
                                                } else if (myTank == meRight) {
                                                    strcpy(dualShotMessage, createDualShotMessage(in_room_id, myId, RIGHT));
                                                } else {
                                                    strcpy(dualShotMessage, createDualShotMessage(in_room_id, myId, LEFT));
                                                }
                                                bytes_sent = send(client_sock, dualShotMessage, strlen(dualShotMessage), 0);
                                                
                                            } else {
                                                dualShotFriend(friendTank, dual_friendRect, friendUp, friendDown, friendRight, friendLeft, 1);
                                            
                                            char dualShotMessage[BUFF_SIZE];
                                                if (friendTank == friendUp) {
                                                    strcpy(dualShotMessage, createDualShotMessage(in_room_id, myId, UP));
                                                } else if (friendTank == friendDown) {
                                                    strcpy(dualShotMessage, createDualShotMessage(in_room_id, myId, DOWN));
                                                } else if (friendTank == friendRight) {
                                                    strcpy(dualShotMessage, createDualShotMessage(in_room_id, myId, RIGHT));
                                                } else {
                                                    strcpy(dualShotMessage, createDualShotMessage(in_room_id, myId, LEFT));
                                                }
                                                 bytes_sent = send(client_sock, dualShotMessage, strlen(dualShotMessage), 0);
                                            }

                                            break;
                                    }
                                break;
                                case GAME_OVER:
                                    switch (e.key.keysym.sym) {
                                        case SDLK_RETURN:
                                            state = CHOOSE_MODE;
                                        break;
                                    }
                                break;
                        }
                    }
                
                    else if (e.type == SDL_MOUSEBUTTONDOWN) {
                        if (state == LOGIN) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            setFocusOnTextbox(&usernameLoginTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&passwordLoginTextbox, mouseX, mouseY);

                            if (isMouseClickOnTexture(mouseX, mouseY, showPasswordTexture, showPasswordRenderQuad)) {
                                showPasswordLogin = !showPasswordLogin;
                            }

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
                                   int id, score;

                                   sscanf(buff, "%d*%d", &id, &score);

                                   myId = id;
                                   single_scores = score;
                                }
                            }
                        }

                        else if (state == REGISTER) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            setFocusOnTextbox(&usernameRegisterTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&passwordRegisterTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&retypePasswordRegisterTextbox, mouseX, mouseY);

                            if (isMouseClickOnTexture(mouseX, mouseY, showRetypePasswordTexture, showRetypedPasswordRenderQuad)) {
                                showRetypePassword = !showRetypePassword;
                            }
                            if (isMouseClickOnTexture(mouseX, mouseY, showPasswordTexture, showPasswordRenderQuad)) {
                                showPassword = !showPassword;
                            }

                            if (mouseX > 230 && mouseX < 430 && mouseY > 380 && mouseY < 430){
                                char registerMessage[BUFF_SIZE];

                                

                                if (strlen(usernameRegisterTextbox.text) == 1 || strlen(passwordRegisterTextbox.text) == 1 || strlen(retypePasswordRegisterTextbox.text) == 1) {
                                    strcpy(register_message, "Please input all fields");
                                } else {
                                    if(strcmp(passwordRegisterTextbox.text,retypePasswordRegisterTextbox.text)!=0){
                                        strcpy(register_message, "Password not match!");
                                    }else{                                      
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
                                            state = LOGIN;
                                        }
                                    }

                                }
                            }
                        }
                        else if (state == CHOOSE_ROOM) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            if (mouseX >= 480 && mouseX <= 520 && mouseY >= 110 && mouseY <= 150) {
                                char createRommMessage[BUFF_SIZE];

                                strcpy(createRommMessage , createCreateRoomMessage(myId));

                                bytes_sent = send(client_sock, createRommMessage, strlen(createRommMessage), 0);

                                bytes_received = recv(client_sock, buff, sizeof(buff), 0);    

                                buff[bytes_received] = '\0';
                                in_room_id = atoi(buff);

                                rooms[in_room_id].status = 1;
                                rooms[in_room_id].first_player_id = myId;

                                state = WAITING_OTHER;

                                isFirstUserInRoom = 1;

                                if (pthread_create(&tid, NULL, receiveThread, (void*)&client_sock) != 0) {
                                    perror("Error creating thread");
                                    return 1;
                                }

                                receiveThreadCreated1 = 1;  // Set the flag to indicate that the thread is created
                                pthread_detach(tid);
                        } else {
                            SDL_GetMouseState(&mouseX, &mouseY);

                            int row = (mouseY - 200) /100;
                            int col = (mouseX - 100) / 120;

                            if (mouseX > 100 && mouseY > 200) {
                                in_room_id = 1 + row*4 + col;
                                
                                if (in_room_id > 0 && in_room_id <= room_nums) {
                                    
                                    if (rooms[in_room_id].status == 1) {

                                        char joinRoomMessage[BUFF_SIZE];

                                        strcpy(joinRoomMessage, createJoinRoomMessage(myId, in_room_id));

                                        bytes_sent = send(client_sock, joinRoomMessage, strlen(joinRoomMessage), 0);

                                        bytes_received = recv(client_sock, buff, sizeof(buff), 0);    

                                        buff[bytes_received] = '\0';

                                        friendId = atoi(buff);
                                        
                                        if (strlen(buff) > 0) {
                                            state = READY_TO_PLAY_DUAL;

                                            if (pthread_create(&pid, NULL, receiveThread1, (void*)&client_sock) != 0) {
                                                perror("Error creating thread");
                                                return 1;
                                            }

                                            receiveThreadCreated2 = 1;  // Set the flag to indicate that the thread is created
                                            pthread_detach(pid);
                            //                 ready_to_start = 1;
                                        }
                                    }
                                }
                            }
                        }
                    } else if (state == CHOOSE_MODE) {
                            SDL_GetMouseState(&mouseX, &mouseY);

                            if (mouseX > 600 && mouseX < 680 && mouseY > 450 && mouseY < 530 ) {

                                char seeRankingMessage[2];

                                snprintf(seeRankingMessage, sizeof(seeRankingMessage), "%c", 0x0e);

                                bytes_sent = send(client_sock, seeRankingMessage, strlen(seeRankingMessage), 0);

                                state = SEEING_RANK;
                            }

                        }else if (state == READY_TO_PLAY_DUAL) {
                            SDL_GetMouseState(&mouseX, &mouseY);

                            if (mouseX > 245 && mouseX < 385 && mouseY > 300 && mouseY < 400) {
                                
                                // myTank = meUp;
                                state = PLAY_DUAL_GAME;

                                char startDualGameMessage[BUFF_SIZE];
                            
                                strcpy(startDualGameMessage, createStartGameMessage(in_room_id));

                                bytes_sent = send(client_sock, startDualGameMessage, strlen(startDualGameMessage), 0);
                            }
                        } else if (state == WAITING_OTHER) {
                            SDL_GetMouseState(&mouseX, &mouseY);

                            if (mouseX > 30 && mouseX < 70 && mouseY > 100 && mouseY < 140) {
                                
                               
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
        int row = (i - 1) / 4;
        int col = (i - 1) % 4;

        SDL_Rect roomRect = {
            100 + col * (roomWidth + spacingX),
            200 + row * (roomHeight + spacingY),
            roomWidth,
            roomHeight
        };

        SDL_RenderCopy(renderer, roomTexture, NULL, &roomRect);

        char roomIdText[2];
        snprintf(roomIdText, sizeof(roomIdText), "%d", rooms[i].id);

        SDL_Surface *textSurface = TTF_RenderText_Solid(font, roomIdText, (SDL_Color){255, 255, 255, 255});
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

        SDL_Rect textRect = {
            roomRect.x+35,
            roomRect.y-25,
            textWidth,
            textHeight
        };

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        char roomStatusText[2]; // Adjust the size as needed
        snprintf(roomStatusText, sizeof(roomIdText), "%d", rooms[i].status);

        SDL_Surface *roomStatusTextSurface = TTF_RenderText_Solid(TINY_FONT, roomStatusText, (SDL_Color){255, 255, 255, 255});
        SDL_Texture *roomStatusTextTexture = SDL_CreateTextureFromSurface(renderer, roomStatusTextSurface);
        SDL_FreeSurface(roomStatusTextSurface);

        SDL_Rect roomStatusRect = {
            roomRect.x+75,
            roomRect.y+85,
            roomStatusTextSurface->w,
            roomStatusTextSurface->h
        };

        SDL_RenderCopy(renderer, roomStatusTextTexture, NULL, &roomStatusRect);
        SDL_Texture *personTexture = IMG_LoadTexture(renderer, "images/person.jpg");

        SDL_Rect personRect = {
            roomRect.x+15,
            roomRect.y+90,
            15,15
        };

        SDL_RenderCopy(renderer, personTexture, NULL, &personRect);
    }
}

void* receiveThread(void* arg) {
    int client_sock = *((int*)arg);
    char buff[BUFF_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);

        buff[bytes_received] = '\0';

        if (buff[0] == 0x0a) { // ready to play

            int friend_id;

            sscanf(buff + 1, "%d", &friend_id); // Skip the first byte (0x05) and read the integer.

            friendId = friend_id;

            readyToStart = 1;
            state = READY_TO_PLAY_DUAL;

        } else if (buff[0] == 0x0b) {
            
            state = PLAY_DUAL_GAME;

        } else if (buff[0] == 0x08) {
            int direction;

            sscanf(buff + 1, "%d", &direction); // Skip the first byte (0x05) and read the integer.

            if (isFirstUserInRoom == 1) {
                if (direction == UP) {
                    friendTank = friendUp;
                    dualMoveUp(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                } else if (direction == DOWN) {
                    friendTank = friendDown;
                    dualMoveDown(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                } else if (direction == RIGHT) {
                    friendTank = friendRight;
                    dualMoveRight(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                } else {
                    friendTank = friendLeft;
                    dualMoveLeft(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                }
            } 
            else {
                if (direction == UP) {
                    myTank = meUp;
                    dualMoveUp(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                } else if (direction == DOWN) {
                    myTank = meDown;
                    dualMoveDown(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                } else if (direction == RIGHT) {
                    myTank = meRight;
                    dualMoveRight(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                } else {
                    myTank = meLeft;
                    dualMoveLeft(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                }
            }
        } else if (buff[0] == 0x0c) {
            dualShotFriend_positive(friendTank, dual_friendRect, friendUp, friendDown, friendRight, friendLeft, 1);
        } else if (buff[0] == 0x0f) {
            char username[BUFF_SIZE];
            int score;
            int rank;

            extractScoreMessage(buff, username, &score, &rank);

            strcpy(topPlayer[rank-1].username, username);
            topPlayer[rank-1].score = score;
        }
    }

    pthread_exit(NULL);
}

void* receiveThread1(void* arg) {
    int client_sock = *((int*)arg);
    char buff[BUFF_SIZE];
    int bytes_received;

    while (1) {
        bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0);

        buff[bytes_received] = '\0';

        if (buff[0] == 0x0a) { // ready to play
            readyToStart = 1;
            state = READY_TO_PLAY_DUAL;

        } else if (buff[0] == 0x0b) {
            
            state = PLAY_DUAL_GAME;

        } else if (buff[0] == 0x08) {
            int direction;

            sscanf(buff + 1, "%d", &direction); // Skip the first byte (0x05) and read the integer.

            if (isFirstUserInRoom == 1) {
                if (direction == UP) {
                    friendTank = friendUp;
                    dualMoveUp(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                } else if (direction == DOWN) {
                    friendTank = friendDown;
                    dualMoveDown(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                } else if (direction == RIGHT) {
                    friendTank = friendRight;
                    dualMoveRight(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                } else {
                    friendTank = friendLeft;
                    dualMoveLeft(&dual_control_hozirontal_controller_friend, &dual_control_vertical_controller_friend, &dual_mode_position_x_friend, &dual_mode_position_y_friend, &dual_friendRect, 8, 5, dual_map_1);
                }
            } 
            else {
                if (direction == UP) {
                    myTank = meUp;
                    dualMoveUp(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                } else if (direction == DOWN) {
                    myTank = meDown;
                    dualMoveDown(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                } else if (direction == RIGHT) {
                    myTank = meRight;
                    dualMoveRight(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                } else {
                    myTank = meLeft;
                    dualMoveLeft(&dual_control_hozirontal_controller, &dual_control_vertical_controller, &dual_mode_position_x, &dual_mode_position_y, &dual_controlRect, 8, 5, dual_map_1);
                }
            }
        } else if (buff[0] == 0x0c) {
            dualShot_positive(myTank, dual_controlRect, meUp, meDown, meRight, meLeft, 1);
        }   
    }

    pthread_exit(NULL);
}

