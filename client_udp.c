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
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int tankX = 220;
int tankY = 245;
int tankWidth = 30;
int tankHeight = 30;
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
SDL_Texture *bullet = NULL;

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


enum AppState {
    MAIN_MENU,
    LOGIN,
    CHOOSE_MODE,
    PLAY_SINGLE_GAME,
    PLAY_DUAL_GAME,
};

void setFocusOnTextbox(Textbox *textbox, int mouseX, int mouseY);

void initTextbox(Textbox *textbox, int x, int y, int w, int h) {
    strcpy(textbox->text, " \0");
    textbox->cursorPosition = 0;
    textbox->boxRect = (SDL_Rect){x, y, w, h};
    textbox->hasFocus = false;
}

void initButton(Button *button, int x, int y, int w, int h, const char *text) {
    strcpy(button->text, text);
    button->boxRect = (SDL_Rect){x, y, w, h};
    button->hasFocus = false;
}

void handleKeyboardEvent(SDL_Event *e, Textbox *textbox) {
    if (!textbox->hasFocus) return;

    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_BACKSPACE && strlen(textbox->text) > 1) {
            // Handle backspace
            textbox->text[strlen(textbox->text) - 1] = '\0';
            printf("delete\n");
        } else if (e->key.keysym.sym == SDLK_RETURN) {
        }
    } else if (e->type == SDL_TEXTINPUT) {
        strcat(textbox->text, e->text.text);
    }
}

int main(){

	int client_sock;
	unsigned char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent,bytes_received, sin_size;
	SDL_Texture *texture = NULL;
	SDL_Texture *loading = NULL;
	SDL_Texture *playing = NULL;
    int menuChoose = 1;
    int chooseMode = 1;
    int single_mode_stage = 1;
    int single_mode_postion_x = 4;
    int single_mode_postion_y = 11;

    enum AppState state = MAIN_MENU; // Start in the main menu state

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
    SDL_Rect controlRect = { 240, 440, 40, 40 };

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
            SDL_Rect renderQuad = { 150, 100, textSurface->w, textSurface->h };
            SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);

            // render login text texture
            SDL_Rect loginTextRenderQuad = { 220, 180, loginText->w, loginText->h };
            SDL_RenderCopy(renderer, loginTextTexture, NULL, &loginTextRenderQuad);

            // render login text texture
            SDL_Rect registerTextRenderQuad = { 220, 230, registerText->w, registerText->h };
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
            enermy_1_right = IMG_LoadTexture(renderer, "images/enermy_1_right.png");
            enermy_1_left = IMG_LoadTexture(renderer, "images/enermy_1_left.png");
            enermy_1_up = IMG_LoadTexture(renderer, "images/enermy_1_up.png");
            enermy_1_down = IMG_LoadTexture(renderer, "images/enermy_1_down.png");
            bullet = IMG_LoadTexture(renderer, "images/bullet.png");

            int tankwidth = 30;
            int tankheight = 30;
            SDL_Rect tankRect = {180, 175, tankwidth, tankheight};

            TTF_Font *loginBigFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 50);  // Adjust font size as needed
            TTF_Font *usernameFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);  // Adjust font size as needed
            TTF_Font *chooseModeFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 35);  // Adjust font size as needed

            if (loginFont == NULL || usernameFont == NULL) {
                printf("Error loading font: %s\n", TTF_GetError());
            }

            SDL_Surface *loginBigText = TTF_RenderText_Solid(loginBigFont, "LOGIN", white);
            SDL_Surface *usernameText = TTF_RenderText_Solid(usernameFont, "Username: ", white);
            SDL_Surface *passwordText = TTF_RenderText_Solid(usernameFont, "Password: ", white);
            SDL_Surface *chooseModeText = TTF_RenderText_Solid(loginBigFont, "Choose Mode", white);
            SDL_Surface *singleModeText = TTF_RenderText_Solid(usernameFont, "1 Person", white);
            SDL_Surface *dualModeText = TTF_RenderText_Solid(usernameFont, "2 People", white);

            SDL_Texture *loginBigTextTexture = SDL_CreateTextureFromSurface(renderer, loginBigText);  
            SDL_Texture *usernameTextTexture = SDL_CreateTextureFromSurface(renderer, usernameText); 
            SDL_Texture *passwordTextTexture = SDL_CreateTextureFromSurface(renderer, passwordText);    
            SDL_Texture *chooseModeTextTexture = SDL_CreateTextureFromSurface(renderer, chooseModeText);  
            SDL_Texture *singleModeTextTexture = SDL_CreateTextureFromSurface(renderer, singleModeText);      
            SDL_Texture *dualModeTextTexture = SDL_CreateTextureFromSurface(renderer, dualModeText);      


            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);  // Black background
            SDL_RenderClear(renderer);

            // Render the text texture
            SDL_Rect loginBigTextRenderQuad = { 195, 100, loginBigText->w, loginBigText->h };
            SDL_Rect usernameRenderQuad = { 120, 200, usernameText->w, usernameText->h };
            SDL_Rect passwordRenderQuad = { 120, 250, passwordText->w, passwordText->h };
            SDL_Rect chooseModeRenderQuad = { 50, 100, chooseModeText->w, chooseModeText->h };
            SDL_Rect singleModeRenderQuad = { 250, 250, singleModeText->w, singleModeText->h };
            SDL_Rect dualModeRenderQuad = { 250, 300, dualModeText->w, dualModeText->h };
            SDL_Rect tankRectChooseMode = {200, 242, tankwidth, tankheight};

            Textbox usernameTextbox;
            Textbox passwordTextbox;
            initTextbox(&usernameTextbox, 320, 195, 200, 30);
            initTextbox(&passwordTextbox, 320, 245, 200, 30);
            
            SDL_Color textColor = {255, 255, 255, 255}; // White text
            createSingleMap1(single_map_1);

            // Create a surface for the text

            while( quit == false ){
                printf("run\n");
                SDL_RenderClear(renderer);
                if (state == MAIN_MENU) {
                    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
                    SDL_RenderCopy(renderer, loginTextTexture, NULL, &loginTextRenderQuad);
                    SDL_RenderCopy(renderer, registerTextTexture, NULL, &registerTextRenderQuad);
                    SDL_RenderCopy(renderer, tank, NULL, &tankRect);
                } else if (state == LOGIN) {
                    SDL_RenderCopy(renderer, loginBigTextTexture, NULL, &loginBigTextRenderQuad);
                    SDL_RenderCopy(renderer, usernameTextTexture, NULL, &usernameRenderQuad);
                    SDL_RenderCopy(renderer, passwordTextTexture, NULL, &passwordRenderQuad);

                    // username box
                    SDL_Surface *usernameBoxTextSurface = TTF_RenderText_Solid(usernameFont, usernameTextbox.text, textColor);
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
                    SDL_Rect usernameBoxRenderQuad = { usernameTextbox.boxRect.x + 5, usernameTextbox.boxRect.y + 5, usernameTextWidth, usernameTextHeight };

                    //password box
                    SDL_Surface *passwordBoxTextSurface = TTF_RenderText_Solid(usernameFont, passwordTextbox.text, textColor);
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
                    SDL_Rect passwordBoxRenderQuad = { passwordTextbox.boxRect.x + 5, passwordTextbox.boxRect.y + 5, passwordTextWidth, passwordTextHeight };

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

                } else if (state == CHOOSE_MODE) {
                    SDL_RenderCopy(renderer, chooseModeTextTexture, NULL, &chooseModeRenderQuad);
                    SDL_RenderCopy(renderer, singleModeTextTexture, NULL, &singleModeRenderQuad);
                    SDL_RenderCopy(renderer, dualModeTextTexture, NULL, &dualModeRenderQuad);
                    SDL_RenderCopy(renderer, tank, NULL, &tankRectChooseMode);
                } else if (state == PLAY_SINGLE_GAME && single_mode_stage == 1) {

                    int mapX = (640 - (12 * 40)) / 2;
                    int mapY = (480 - (12 * 40)) / 2;

                    // Render the map
                    for (int y = 0; y < 12; y++) {
                        for (int x = 0; x < 12; x++) {
                            SDL_Rect destRect = { mapX + x * 40, mapY + y * 40, 40, 40 };
                            SDL_Texture* texture = NULL;

                            switch (single_map_1[y][x]) {
                                case 0:
                                    break;
                                case 1:
                                    texture = block;
                                    break;
                                case 2:
                                    texture = iron;
                                    break;
                                case 9:
                                    texture = meUp;
                                    break;
                                case 7: 
                                    texture = enermy_1_right;
                                    break;
                                default:
                                    break;
                            }

                            if (texture == meUp) {
                                SDL_RenderCopy(renderer, texture, NULL, &controlRect);
                            } else {
                                SDL_RenderCopy(renderer, texture, NULL, &destRect);
                            }
                        }
                    }
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
                                        if (tankRect.y < 225) {

                                            tankRect.y += 50;
                                            menuChoose = 2;
                                        }
                                        break;
                                    case SDLK_UP:
                                        if (tankRect.y > 175) {

                                            tankRect.y -= 50;
                                            menuChoose = 1;
                                        }
                                        break;
                                    case SDLK_RETURN:
                                        if (menuChoose == 1) {
                                            state = LOGIN;

                                        } else {

                                        }

                                        break;
                                }
                            break;
                            case LOGIN:
                                handleKeyboardEvent(&e, &usernameTextbox);
                                handleKeyboardEvent(&e, &passwordTextbox);
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
                                            if (menuChoose == 1) {
                                                state = PLAY_SINGLE_GAME;

                                            } else {

                                            }

                                            break;
                                }
                            case PLAY_SINGLE_GAME:
                                switch (e.key.keysym.sym) {
                                    case SDLK_DOWN:
                                        
                                        break;
                                    case SDLK_UP:
                                        
                                        break;
                                    case SDLK_RETURN:
                                       
                                        break;
                                }
                                break;
                        }
                    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                        if (state == LOGIN) {
                            int mouseX, mouseY;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            setFocusOnTextbox(&usernameTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&passwordTextbox, mouseX, mouseY);

                            if (mouseX >= 230 && mouseX <= 430 && mouseY >= 330 && mouseY <= 370) {
                                char loginMessage[BUFF_SIZE];
                                strcpy(loginMessage, createLoginMessage(usernameTextbox.text, passwordTextbox.text));

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

                                if (strcmp(buff, "found") == 0) {
                                    state = CHOOSE_MODE;
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
        printf("focus\n");
    } else {
        textbox->hasFocus = false; // Mouse click is outside the textbox
    }
}

void login(char username[], char password[]){

}
