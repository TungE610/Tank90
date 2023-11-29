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
#include <errno.h>
#define PORT 5550
#define BUFF_SIZE 1024
#define MAX_CLIENTS 10

int number_of_players = 0;
int server_sock;

struct Player players[MAX_PLAYERS];
struct Room rooms[MAX_PLAYERS];

// Function to handle communication with a client
void *handleClient(void *arg) {
	int sin_size = sizeof(struct sockaddr_in);
    struct Player *player = (struct Player *)arg;
    struct sockaddr_in clientAddress = player->address;
    player->is_choosing_game_mode = 1;

    char buff[BUFF_SIZE];
    int bytes_received, bytes_sent;

    free(arg);
    pthread_detach(pthread_self());

    while (1) {
        // Use MSG_DONTWAIT to make recvfrom non-blocking
        bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, MSG_DONTWAIT, (struct sockaddr *)&clientAddress, &sin_size);

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
            printf("receive from client: %s\n", buff);
            player->is_choosing_game_mode = 0;

            // Handle the received message (you can implement your logic here)

            // For example, check if the message is "up" or "down"
            if (strcmp(buff, "1 person") == 0) {
                player->game_mode = 1;
            } else if (strcmp(buff, "2 people") == 0) {
                player->game_mode = 2;
                player->is_waiting_other_player = 1;
                for (int i = 0; i < MAX_PLAYERS; i ++) {
                    if (players[i].is_waiting_other_player == 1 && players[i].id != player->id ) {
                        players[i].is_waiting_other_player = 0;
                        player->is_waiting_other_player = 0;

                        bytes_sent = sendto(server_sock, "play", strlen("play"), 0, (struct sockaddr *) &player->address, sin_size );
                        if (bytes_sent < 0)
                            perror("\nError: ");	

                        bytes_sent = sendto(server_sock, "play", strlen("play"), 0, (struct sockaddr *) &players[i].address, sin_size );
                        if (bytes_sent < 0)
                            perror("\nError: ");

                        break;
                    }
                }
            }
        }
    }

    // Clean up resources and exit the thread
    free(player);
    pthread_exit(NULL);
}

int main() {
    char buff[BUFF_SIZE];
    int bytes_received, bytes_sent;
    struct sockaddr_in server;
    struct sockaddr_in client;
    int sin_size;

    // Create an array to store thread IDs for each client
    pthread_t threadIDs[MAX_CLIENTS];

    // Step 1: Construct a UDP socket
    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("\nError: ");
        exit(0);
    }

    // Step 2: Bind address to socket
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server.sin_zero), 8);

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        perror("\nError: ");
        exit(0);
    }
	sin_size = sizeof(struct sockaddr_in);

    // Step 3: Communicate with clients
    while (1) {
        struct Player *player = (struct Player *)malloc(sizeof(struct Player));
        player->id = number_of_players;
		// Receive data from the client
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&player->address, &sin_size);

		if (bytes_received < 0) {
			perror("\nError receiving data: ");
			free(player);
			continue;
		}

		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(player->address.sin_addr), client_ip, INET_ADDRSTRLEN);
		int client_port = ntohs(player->address.sin_port);

		printf("[%s:%d] (%d): %s\n", client_ip, client_port, bytes_received, buff);

        pthread_create(&threadIDs[number_of_players], NULL, handleClient, (void *)player);
        // Create a new thread to handle the client
            // Increment the number of players (clients)
        pthread_detach(threadIDs[number_of_players]);
        number_of_players++;
        if (number_of_players >= MAX_CLIENTS) {
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
