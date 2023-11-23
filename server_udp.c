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

#define PORT 5550
#define BUFF_SIZE 1024
#define MAX_CLIENTS 10

int number_of_players = 0;
int server_sock;

// Structure to hold client information
struct ClientInfo {
    int socket;
    struct sockaddr_in address;
    struct Tank tank;
};

struct ClientInfo clients[MAX_PLAYERS];

void broadcastTankUpdate(char *key, struct ClientInfo *sender) {
    printf("check key: %s", key);
    for (int i = 0; i < 2; i++) {
        clients[i].socket = server_sock;
    }
    for (int i = 0; i < number_of_players; i++) {
        if (clients[i].socket != sender->socket) {
            sendto(server_sock, key, strlen(key), 0, (struct sockaddr *)&clients[i].address, sizeof(struct sockaddr_in));
        }
    }
}

// Function to handle communication with a client
void *handleClient(void *arg) {
	int sin_size = sizeof(struct sockaddr_in);
    struct ClientInfo *clientInfo = (struct ClientInfo *)arg;
    int clientSocket = clientInfo->socket;
    struct sockaddr_in clientAddress = clientInfo->address;

    char buff[BUFF_SIZE];
    int bytes_received, bytes_sent;

    while (1) {
		bytes_received = recvfrom(clientSocket, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&clientAddress, &sin_size);

        if (bytes_received < 0)
            perror("\nError: ");
        else if (bytes_received == 0) {
            // Client disconnected
            printf("Client [%s:%d] disconnected.\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
            break;
        } else {
            buff[bytes_received] = '\0';
            printf("[%s:%d]: %s\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), buff);


            if (strcmp(buff, "down") == 0) {
                clientInfo->tank.y -= 33; // Adjust the value as needed
            } else if (strcmp(buff, "up") == 0) {
                clientInfo->tank.y += 33; // Adjust the value as needed
            }
            // Process the received message (e.g., handle "up" and "down" messages)
            broadcastTankUpdate(buff, clientInfo);
        }
    }

    // Clean up resources and exit the thread
    close(clientSocket);
    free(clientInfo);
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

    printf("server sock: %d", server_sock);

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
        struct ClientInfo *clientInfo = (struct ClientInfo *)malloc(sizeof(struct ClientInfo));

		// Receive data from the client
		clientInfo->socket = server_sock; // The socket is the server socket for UDP
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&clientInfo->address, &sin_size);

		if (bytes_received < 0) {
			perror("\nError receiving data: ");
			free(clientInfo);
			continue;
		}

		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientInfo->address.sin_addr), client_ip, INET_ADDRSTRLEN);
		int client_port = ntohs(clientInfo->address.sin_port);

		char player_id[10];
		int chars_written = snprintf(player_id, sizeof(player_id), "%d", number_of_players);

		printf("[%s:%d] (%d): %s\n", client_ip, client_port, bytes_received, buff);

		bytes_sent = sendto(server_sock, player_id, strlen(player_id), 0, (struct sockaddr *) &clientInfo->address, sin_size);
		if (bytes_sent < 0)
			perror("\nError: ");


        // Create a new thread to handle the client
        if (pthread_create(&threadIDs[number_of_players], NULL, handleClient, (void *)clientInfo) != 0) {
            perror("\nError creating thread: ");
            close(clientInfo->socket);
            free(clientInfo);
        } else {
            // Increment the number of players (clients)
            number_of_players++;
            if (number_of_players >= MAX_CLIENTS) {
                printf("Maximum number of clients reached. No more connections will be accepted.\n");
                break;
            }
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < number_of_players; i++) {
        pthread_join(threadIDs[i], NULL);
    }

    close(server_sock);
    return 0;
}
