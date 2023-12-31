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

#define SERV_IP "127.0.0.1"
#define SERV_PORT 5500
#define BUFF_SIZE 1024
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int tankX = 220;
int tankY = 245;
int tankWidth = 30;
int tankHeight = 30;
SDL_Texture *tank = NULL;
SDL_Renderer *renderer;
char *SERVER_ADDR = "127.0.0.1";

typedef struct {
    char text[256];
    int cursorPosition;
    SDL_Rect boxRect; // Position and size of the textbox
    bool hasFocus;
} Textbox;

enum AppState {
    MAIN_MENU,
    LOGIN,
    GAME_PLAY,
};

void setFocusOnTextbox(Textbox *textbox, int mouseX, int mouseY);

void initTextbox(Textbox *textbox, int x, int y, int w, int h) {
    strcpy(textbox->text, " \0");
    textbox->cursorPosition = 0;
    textbox->boxRect = (SDL_Rect){x, y, w, h};
    textbox->hasFocus = false;
}

void handleKeyboardEvent(SDL_Event *e, Textbox *textbox) {
    if (!textbox->hasFocus) return;

    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_BACKSPACE && textbox->cursorPosition > 0) {
            // Handle backspace
            textbox->text[--textbox->cursorPosition] = '\0';
            printf("delete\n");
        } else if (e->key.keysym.sym == SDLK_RETURN) {
            // Handle enter key (submit the form or switch to next field)
        }
    } else if (e->type == SDL_TEXTINPUT) {
        if (textbox->cursorPosition < sizeof(textbox->text) - 1) {
            // Append new character
            textbox->text[textbox->cursorPosition++] = e->text.text[0];
            textbox->text[textbox->cursorPosition] = '\0';
        }
    }
}

void load_login_texture(SDL_Renderer *renderer);

int main(){

	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent,bytes_received, sin_size;
	SDL_Texture *texture = NULL;
	SDL_Texture *loading = NULL;
	SDL_Texture *playing = NULL;
    int message = LOGIN_MESSAGE;

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
            int tankwidth = 30;
            int tankheight = 30;
            SDL_Rect tankRect = {180, 175, tankwidth, tankheight};

            TTF_Font *loginBigFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 50);  // Adjust font size as needed
            TTF_Font *usernameFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 20);  // Adjust font size as needed

            if (loginFont == NULL || usernameFont == NULL) {
                printf("Error loading font: %s\n", TTF_GetError());
            }

            SDL_Surface *loginBigText = TTF_RenderText_Solid(loginBigFont, "LOGIN", white);
            SDL_Surface *usernameText = TTF_RenderText_Solid(usernameFont, "Username: ", white);
            SDL_Surface *passwordText = TTF_RenderText_Solid(usernameFont, "Password: ", white);

            SDL_Texture *loginBigTextTexture = SDL_CreateTextureFromSurface(renderer, loginBigText);  
            SDL_Texture *usernameTextTexture = SDL_CreateTextureFromSurface(renderer, usernameText); 
            SDL_Texture *passwordTextTexture = SDL_CreateTextureFromSurface(renderer, passwordText);      

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);  // Black background
            SDL_RenderClear(renderer);

            // Render the text texture
            SDL_Rect loginBigTextRenderQuad = { 195, 100, loginBigText->w, loginBigText->h };
            SDL_Rect usernameRenderQuad = { 120, 200, usernameText->w, usernameText->h };
            SDL_Rect passwordRenderQuad = { 120, 250, passwordText->w, passwordText->h };

            Textbox usernameTextbox;
            initTextbox(&usernameTextbox, 320, 200, 200, 30);
            
            SDL_Color textColor = {255, 255, 255, 255}; // White text

            // Create a surface for the text
            SDL_Surface *boxTextSurface = TTF_RenderText_Solid(usernameFont, usernameTextbox.text, textColor);
            if (boxTextSurface == NULL) {
                // Handle the error, e.g., log it and return
                printf("Error rendering text: %s\n", TTF_GetError());
            }

            // Create a texture from the surface
            SDL_Texture *boxTextTexture = SDL_CreateTextureFromSurface(renderer, boxTextSurface);
            if (textTexture == NULL) {
                // Handle the error, e.g., log it and return
                printf("Error creating texture: %s\n", SDL_GetError());
                SDL_FreeSurface(boxTextSurface); // Free the surface to avoid memory leak
            }

            // Set rendering space and render to screen
            int textWidth = boxTextSurface->w;
            int textHeight = boxTextSurface->h;
            SDL_Rect boxRenderQuad = { usernameTextbox.boxRect.x + 5, usernameTextbox.boxRect.y + 5, textWidth, textHeight };

            while( quit == false ){
                // render menu 
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

                    SDL_RenderCopy(renderer, boxTextTexture, NULL, &boxRenderQuad);
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
                                            message = REGISTER_MESSAGE;
                                        }
                                        break;
                                    case SDLK_UP:
                                        if (tankRect.y > 175) {

                                            tankRect.y -= 50;
                                            message = LOGIN_MESSAGE;
                                        }
                                        break;
                                    case SDLK_RETURN:
                                        if (message == LOGIN_MESSAGE) {
                                            state = LOGIN;

                                        } else {

                                        }

                                        break;
                                }
                            break;
                            case LOGIN:
                                handleKeyboardEvent(&e, &usernameTextbox);
                            break;
                        }
                    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                        if (state == LOGIN) {
                            int mouseX, mouseY;
                            SDL_GetMouseState(&mouseX, &mouseY);
                            setFocusOnTextbox(&usernameTextbox, mouseX, mouseY);
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

