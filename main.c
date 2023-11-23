#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <SDL2/SDL_image.h>
#include "constants.h"
#include "objects.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
struct Player players[MAX_PLAYERS];
int number_of_players = 0;

SDL_Texture* load_texture(SDL_Renderer *renderer, char *file) {
    SDL_Surface *bitmap = NULL;
    SDL_Texture *texture = NULL;
    bitmap = SDL_LoadBMP(file);
    texture = SDL_CreateTextureFromSurface(renderer, bitmap);
    SDL_FreeSurface(bitmap);
    return texture;
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    int sock_server, sock_client;
    
    SDL_Texture *texture = NULL;
    SDL_Texture *tank = NULL;
    SDL_Renderer *renderer;

    TTF_Init();

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
            texture = IMG_LoadTexture(renderer, "images/welcome.png");
            tank = IMG_LoadTexture(renderer, "images/player_green_up.png");
            int tankwidth = 30;
            int tankheight = 30;
            SDL_Rect tankRect = {220, 245, tankwidth, tankheight};

            while( quit == false ){
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderCopy(renderer, tank, NULL, &tankRect);
                SDL_RenderPresent(renderer);
                
                while( SDL_PollEvent( &e ) ){ 
                    if( e.type == SDL_QUIT ) 
                    quit = true;

                    resolve_keyboard(e, &players[1]);
                }
                
            send_to_server(sock_client, server_addr, my_id, key_state_from_player(&players[my_id]));


            }
        }
    }
 
    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}