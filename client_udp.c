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
const int SCREEN_HEIGHT = 540;
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
SDL_Texture *none = NULL;

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
    REGISTER,
    CHOOSE_MODE,
    PLAY_SINGLE_GAME,
    PLAY_DUAL_GAME,
};

int totalBullets = 20;

void setFocusOnTextbox(Textbox *textbox, int mouseX, int mouseY);
void moveDown(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect);
void moveUp(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect);
void moveRight(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect);
void moveLeft(int *position_x, int *position_y,int *coor_x, int *coor_y, SDL_Rect *controlRect);

void initTextbox(Textbox *textbox, int x, int y, int w, int h) {
    strcpy(textbox->text, " \0");
    textbox->cursorPosition = 0;
    textbox->boxRect = (SDL_Rect){x, y, w, h};
    textbox->hasFocus = false;
}

void updatePlayerPosition(int *y, int *x, int deltaY, int deltaX) {
    single_map_1[*y][*x] = 0;
    *y += deltaY;
    *x += deltaX;
    single_map_1[*y][*x] = 9;
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

int main(){

	int client_sock;
	unsigned char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_sent,bytes_received, sin_size;
	SDL_Texture *texture = NULL;
	SDL_Texture *loading = NULL;
	SDL_Texture *playing = NULL;
    int single_mode_postion_x = 4;
    int single_mode_postion_y = 11;
    int menuChoose = 1;
    int chooseMode = 1;
    int single_mode_stage = 1;
    char login_message[BUFF_SIZE];
    char register_message[BUFF_SIZE];
    strcpy(login_message, " ");
    strcpy(register_message, " ");
    int mouseX, mouseY;
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
            enermy_1_right = IMG_LoadTexture(renderer, "images/enermy_1_right.png");
            enermy_1_left = IMG_LoadTexture(renderer, "images/enermy_1_left.png");
            enermy_1_up = IMG_LoadTexture(renderer, "images/enermy_1_up.png");
            enermy_1_down = IMG_LoadTexture(renderer, "images/enermy_1_down.png");
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
            SDL_Surface *usernameText = TTF_RenderText_Solid(usernameFont, "Username: ", white);
            SDL_Surface *retypePasswordText = TTF_RenderText_Solid(usernameFont, "Retype Password: ", white);
            SDL_Surface *passwordText = TTF_RenderText_Solid(usernameFont, "Password: ", white);
            SDL_Surface *chooseModeText = TTF_RenderText_Solid(loginBigFont, "Choose Mode", white);
            SDL_Surface *singleModeText = TTF_RenderText_Solid(usernameFont, "1 Person", white);
            SDL_Surface *dualModeText = TTF_RenderText_Solid(usernameFont, "2 People", white);

            SDL_Texture *loginBigTextTexture = SDL_CreateTextureFromSurface(renderer, loginBigText); 
            SDL_Texture *registerBigTextTexture = SDL_CreateTextureFromSurface(renderer, registerBigText);  \
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
    
            SDL_Color textColor = {255, 255, 255, 255}; // White text
            createSingleMap1(single_map_1);

            // Create a surface for the text
            int vertical_controller = 88;
            int hozirontal_controller = 32;

            TTF_Font *loginMessageFont = TTF_OpenFont("./resources/font/ARCADE.TTF", 13);  // Adjust font size as needed
            SDL_Texture *myTank = meUp;

            Bullet bullet[totalBullets];
            SDL_Rect bulletRect[totalBullets];

            // Initialize bullets and rects
            for (int i = 0; i < totalBullets; i++) {
                bullet[i].is_active = 0;  // Assuming first bullet is active
                bulletRect[i] = (SDL_Rect){ controlRect.x, controlRect.y, 30, 30 };
            }

            while( quit == false ){
                // SDL_Texture *bulletTexture[totalBullets];

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

                for (int i = 0; i < totalBullets; i ++) {
                    if (bullet[i].is_active == 1) {
                        SDL_RenderCopy(renderer, bulletTexture[i], NULL, &bulletRect[i]);
                    }
                    if (bullet[i].is_active == 1) {
                        if (bullet[i].direction == UP) {
                            bulletRect[i].y -= 8;
                        } else if (bullet[i].direction == DOWN) {
                            bulletRect[i].y += 8;
                        } else if (bullet[i].direction == RIGHT) {
                            bulletRect[i].x += 8;
                        } else {
                            bulletRect[i].x -= 8;
                        }
                        SDL_Delay(25);
                        usleep(25);
                    }

                    if (bulletRect[i].x > 500) {
                        bullet[i].is_active = 0;
                        SDL_DestroyTexture(bulletTexture[i]); // Free the surface to avoid memory leak
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
                                            if (menuChoose == 1) {
                                                state = PLAY_SINGLE_GAME;

                                            } else {

                                            }

                                            break;
                                }
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

                                printf("receive from server: %s\n", buff);

                                if (strcmp(buff, "found") == 0) {
                                    state = CHOOSE_MODE;
                                } else if (strcmp(buff, "locked") == 0){
                                    strcpy(login_message, "Your account was locked");
                                } else if (strcmp(buff, "logged") == 0) {
                                    strcpy(login_message, "Your account was logged somewhere");
                                } else if (strcmp(buff, "invalid") == 0) {
                                    strcpy(login_message, "username or password incorrect");
                                } else {  
                                   strcpy(login_message, "Account not found");
                                }
                            }
                        } else if (state == REGISTER) {
                            SDL_GetMouseState(&mouseX, &mouseY);
                            setFocusOnTextbox(&usernameRegisterTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&passwordRegisterTextbox, mouseX, mouseY);
                            setFocusOnTextbox(&retypePasswordRegisterTextbox, mouseX, mouseY);
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

void moveRight(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *controlRect) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    // Check if moving right is possible and handle the cases
    if (horizontal_controller % 8 == 0 && *coor_x + 1 < 12 && single_map_1[*coor_y][*coor_x + 1] == 0) {
        if (vertical_controller % 8 != 0 && vertical_controller < *coor_y * 8) {
            if (single_map_1[*coor_y][*coor_x + 1] == 0 && single_map_1[*coor_y - 1][*coor_x + 1] == 0) {
                ++horizontal_controller;
                controlRect->x += 5;
            }
        } else if (vertical_controller % 8 != 0 && vertical_controller > *coor_y * 8) {
            if (single_map_1[*coor_y + 1][*coor_x + 1] == 0 && single_map_1[*coor_y][*coor_x + 1] == 0) {
                ++horizontal_controller;
                controlRect->x += 5;
            }
        } else if (vertical_controller % 8 == 0) {
            ++horizontal_controller;
            controlRect->x += 5;
        }
    } else if (horizontal_controller % 8 == 7 && horizontal_controller + 1 == (*coor_x + 1) * 8) {
        ++horizontal_controller;
        controlRect->x += 5;
        updatePlayerPosition(coor_y, coor_x, 0, 1);
    } else if (horizontal_controller % 8 != 0) {
        ++horizontal_controller;
        controlRect->x += 5;
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
