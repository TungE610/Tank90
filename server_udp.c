#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "constants.h"
#include "objects.h"
#include "utils.h"
#include "messages.h"
#include <errno.h>

#define PORT 5500
#define BACKLOG 20   /* Number of allowed connections */

int number_of_players = 0;
int server_sock;

struct Player *players[MAX_PLAYERS];
struct Room rooms[MAX_PLAYERS];
void login(singleLinkedList*list,char message[], int socket);

// Function to handle communication with a client
void *handleClient(void *arg) {
	int sin_size = sizeof(struct sockaddr_in);
    struct Player *player = (struct Player *)arg;
    players[player->id] = player;

    player->is_choosing_game_mode = 1;
    
    struct sockaddr_in clientAddress = player->address;

    char buff[BUFF_SIZE];
    int bytes_received, bytes_sent;

    while (1) {
        // Use MSG_DONTWAIT to make recvfrom non-blocking
	    bytes_received = recv(player->socket, buff, BUFF_SIZE, 0); //blocking

        if (bytes_received < 0) {
            // Check if no data is available
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available, continue the loop
                continue;
            } else {
                perror("\nError: ");
                break;
            }
        } else if (bytes_received == 0) {
            // Client disconnected
            printf("Client [%s:%d] disconnected.\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
            player->is_choosing_game_mode = 0;
            break;
        } else {
            buff[bytes_received] = '\0';

            if (buff[0] == 0x01) {
                login(player->list, buff, player->socket);
            }

            // player->is_choosing_game_mode = 0;

            // Handle the received message (you can implement your logic here)

            // For example, check if the message is "up" or "down"
            // if (strcmp(buff, "1 person") == 0) {
            //     player->game_mode = 1;
            // } else if (strcmp(buff, "2 people") == 0) {
            //     player->game_mode = 2;
            //     player->is_waiting_other_player = 1;
            //     for (int i = 0; i < number_of_players; i ++) {
            //         printf("player %d: %d", i, players[i]->is_waiting_other_player);
            //     }

            //     for (int i = 0; i < number_of_players; i ++) {
            //         if (players[i]->is_waiting_other_player == 1 && players[i]->id != player->id ) {
            //             players[i]->is_waiting_other_player = 0;
            //             player->is_waiting_other_player = 0;

            //             bytes_sent = send(player->socket, "play", strlen("play"), 0);

            //             bytes_sent = send(players[i]->socket, "play", strlen("play"), 0);

            //             break;
            //         }
            //     }
            // }
        }
    }

    // Clean up resources and exit the thread
    close(player->socket);
    pthread_exit(NULL);
}

int main() {
    char buff[BUFF_SIZE];
    int bytes_received, bytes_sent;
    struct sockaddr_in server;
    struct sockaddr_in *client;
    int sin_size;
    int *client_sock;
    singleLinkedList *list = createSingleList();
    readDatatoList(list);
    
    // Create an array to store thread IDs for each client
    pthread_t threadIDs[MAX_PLAYERS];

    // Step 1: Construct a UDP socket

	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}

    // Step 2: Bind address to socket
    bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT); 
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   

	if(bind(server_sock,(struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}

	if(listen(server_sock, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}

	sin_size = sizeof(struct sockaddr_in);
	client = malloc(sin_size);

    // Step 3: Communicate with clients
    while (1) {
        client_sock = malloc(sizeof(int));

        struct Player *player = (struct Player *)malloc(sizeof(struct Player));
        player->id = number_of_players;
        player->list = list;

		// Receive data from the client
	    if ((*client_sock = accept(server_sock, (struct sockaddr *)client, &sin_size)) ==- 1)
			perror("\nError: ");
        
        player->socket = *client_sock;

		printf("You got a connection from %s\n", inet_ntoa(client->sin_addr) ); /* prints client's IP */

        pthread_create(&threadIDs[number_of_players], NULL, handleClient, (void *)player);

        pthread_detach(threadIDs[number_of_players]);

        number_of_players++;
        if (number_of_players >= MAX_PLAYERS) {
            printf("Maximum number of clients reached. No more connections will be accepted.\n");
            break;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < number_of_players; i++) {
        pthread_join(threadIDs[i], NULL);
    }

    close(server_sock);
    return 0;
}

void login(singleLinkedList* list,char  message[], int socket){
    char *extractedUsername;
    char *extractedPassword;
    int bytes_sent;

    node* foundPlayer;
    extractUsernameAndPassword((unsigned char *)message, &extractedUsername, &extractedPassword);
    printf("username: %s\n",extractedUsername);
    printf("password: %s\n",extractedPassword);

    foundPlayer = findAccount(list, extractedUsername);

    if (foundPlayer != NULL) {
        
        if (foundPlayer->element.status == 1 && foundPlayer->element.logged == 0) {

            if (strcmp(foundPlayer->element.password, extractedPassword) == 0) {
                bytes_sent = send(socket, "found", strlen("found"), 0);
            } else {
                bytes_sent = send(socket, "invalid", strlen("invalid"), 0);
            }

        } else if (foundPlayer->element.status == 0) {

            bytes_sent = send(socket, "locked", strlen("locked"), 0);
        } else if (foundPlayer->element.status == 1 && foundPlayer->element.logged == 1) {

            bytes_sent = send(socket, "logged", strlen("logged"), 0);
        }
    } else {
        bytes_sent = send(socket, "not found", strlen("not found"), 0);
    }
}
