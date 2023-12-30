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

#define SERV_IP "127.0.0.1"
#define SERV_PORT 5500
#define BUFF_SIZE 1024
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int tankX = 220;
int tankY = 245;
int tankWidth = 30;
int tankHeight = 30;
char message[BUFF_SIZE];
SDL_Texture *tank = NULL;
SDL_Renderer *renderer;
char *SERVER_ADDR = "127.0.0.1";

int main(){

	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent,bytes_received, sin_size;
	SDL_Texture *texture = NULL;
	SDL_Texture *loading = NULL;
	SDL_Texture *playing = NULL;

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

    // Step 4: Receive data from the server
    // bytes_received = recvfrom(client_sock, buff, BUFF_SIZE, 0,(struct sockaddr *) &server_addr, &sin_size);    
    // if (bytes_received < 0) {
    //     perror("Error: ");
    //     close(client_sock);
    //     return 0;
    // }

    // buff[bytes_received] = '\0';
    // player_id = atoi(buff);
    // printf("player_id: %d", player_id);
	
    // pthread_mutex_t mutex;
    // if (pthread_mutex_init(&mutex, NULL) != 0) {
    //     perror("Mutex initialization failed");
    //     close(client_sock);
    //     return 1;
    // }
	 //The window we'll be rendering to
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
            TTF_Font *font = TTF_OpenFont("./resources/font/ARCADE.TTF", 5);  // Adjust font size as needed
            if (font == NULL) {
                printf("Error loading font: %s\n", TTF_GetError());
                return 1;
            }

            // Create a text surface
            SDL_Color white = {255, 255, 255};  // White text color
            SDL_Surface *textSurface = TTF_RenderText_Solid(font, "TANK 90", white);
            if (textSurface == NULL) {
                printf("Error creating text surface: %s\n", TTF_GetError());
                TTF_CloseFont(font);
                return 1;
            }

            // Create a texture from the text surface
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);      
            
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 25);  // Black background
            SDL_RenderClear(renderer);

            // Render the text texture
            SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
            SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);


            // load tank icon to choosee
            tank = IMG_LoadTexture(renderer, "images/player_green_up.png");
            int tankwidth = 30;
            int tankheight = 30;
            SDL_Rect tankRect = {220, 245, tankwidth, tankheight};
            
            while( quit == false ){
                // render menu 
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderCopy(renderer, tank, NULL, &tankRect);
                SDL_RenderPresent(renderer);
                
                while( SDL_PollEvent( &e ) ){ 
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    } else if (e.type == SDL_KEYDOWN) {
                        switch (e.key.keysym.sym) {
                            case SDLK_DOWN:
                                if (tankRect.y <= 245) {
                                    tankRect.y += 33; // Adjust the value as needed
                                    snprintf(message, sizeof(message), "2 people");
                                }
                                printf("pressed down\n");
                                break;
                            case SDLK_UP:
                                if (tankRect.y >= 278) {
                                    tankRect.y -= 33; // Adjust the value as needed
                                    snprintf(message, sizeof(message), "1 person");
                                }
                                printf("pressed up\n");
                                break;
                            case SDLK_RETURN:
                                    bytes_sent = send(client_sock, message, strlen(message), 0);                                    if (bytes_sent < 0) {
                                        perror("Error sending message: ");
                                    }    
                                    
                                    loading = IMG_LoadTexture(renderer, "images/loading1.jpg");

                                    // Destroy old textures and surface
                                    SDL_DestroyTexture(texture);
                                    SDL_DestroyTexture(tank);


                                    SDL_RenderCopy(renderer, loading, NULL, NULL);
                                    SDL_RenderPresent(renderer);

                                    bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
                                    if (bytes_received < 0) {
                                        perror("Error: ");
                                    }
                                    
                                    buff[bytes_received] = '\0';
                                    printf("buff: %s", buff);
                                    if (strcmp(buff, "play") == 0) {
                                        playing = IMG_LoadTexture(renderer, "images/map.png");
                                        SDL_DestroyTexture(loading);

                                        SDL_RenderCopy(renderer, playing, NULL, NULL);
                                        SDL_RenderPresent(renderer);
                                    }

                                    break;                                 
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