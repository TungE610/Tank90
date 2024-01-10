#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
int room_nums = 0;
int server_sock;
pthread_mutex_t room_nums_mutex;

struct Player *players[MAX_PLAYERS];
Room rooms[MAX_PLAYERS];
void registerPlayer(singleLinkedList* list,char message[], int socket, int id);
void login(singleLinkedList* list,char message[], int socket, int id);

void initRoom(Room *room, int first_player_id) {
    room->id = room_nums;
    room->first_player_id = first_player_id;
    room->second_player_id = 0;
    room->status = 1;
}

void checkRooms(){
    for (int i = 1; i <= room_nums; i ++) {
        printf("%d %d %d %d\n", rooms[i].id, rooms[i].first_player_id, rooms[i].second_player_id, rooms[i].status);
    }
}

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
                login(player->list, buff, player->socket, player->id);

            } else if (buff[0] == 0x02) {
                
                node* root = player->list->root;
                int count = 0;
                while(root != NULL) {
                    count ++;
                    root = root->next;
                }
                
                registerPlayer(player->list, buff, player->socket,  count + 1);

            } else if (buff[0] == 0x03) {

                pthread_mutex_lock(&room_nums_mutex);
                room_nums += 1;
                pthread_mutex_unlock(&room_nums_mutex);
            
                char player_id[3]; // Allocate memory for player_id to store up to 2 digits and a null terminator
                memcpy(player_id, buff + 1, strlen(buff));

                int iplayer_id = atoi(player_id);

                // Room *new_room = malloc(sizeof(Room)); // Allocate memory for new_room
                initRoom(&rooms[room_nums], iplayer_id);
                // &rooms[room_nums] = new_room;
                char snum[2];

                sprintf(snum, "%d", room_nums);

                bytes_sent = send(player->socket, snum, strlen(snum), 0);

            } else if (buff[0] == 0x04) {
                
                int waitingRoom = 0;

                for (int i =0 ;i <= room_nums; i ++) {
                    if (rooms[i].status == 1 || rooms[i].status == 2) {
                        waitingRoom++;
                    }
                }

                char swaitingRoom[BUFF_SIZE];

                if (waitingRoom == 0) {
                    sprintf(swaitingRoom, "%d", 0);
                } else {
                    sprintf(swaitingRoom, "%d", waitingRoom);
                }
                
                bytes_sent = send(player->socket, swaitingRoom, strlen(swaitingRoom), 0);

                bytes_received = recv(player->socket, buff, BUFF_SIZE, 0); //blocking

                buff[bytes_received] = '\0';

                if (strcmp(buff, "ok") == 0) {
                    if (waitingRoom > 0) {
                        for (int i = 0 ;i <= room_nums; i ++) {

                            if (rooms[i].status == 1 || rooms[i].status == 2) {
                                char roomStateMessage[BUFF_SIZE];
                                
                                if (rooms[i].status == 1) {
                                    strcpy(roomStateMessage, createRoomStateMessage(i, rooms[i].status, rooms[i].first_player_id, 0));

                                } else if (rooms[i].status == 2) {
                                    strcpy(roomStateMessage, createRoomStateMessage(i, rooms[i].status, rooms[i].first_player_id, rooms[i].second_player_id));

                                }
                                bytes_sent = send(player->socket, roomStateMessage, strlen(roomStateMessage), 0);
                            }
                        }
                    }
                }
            } else if (buff[0] == 0x05) {
                int playerId, roomId;

                int res = extractJoinRoomMessage(buff, &playerId, &roomId);

                rooms[roomId].second_player_id = playerId;

                rooms[roomId].status = 2;

                checkRooms();

                bytes_sent = send(player->socket, "ok", strlen("ok"), 0);

                int waitingPlayerId;

                for (int i = 0; i < number_of_players; i ++) {
                    if (players[i]->system_id == rooms[roomId].first_player_id) {
                        waitingPlayerId = i;
                        break;
                    }
                }

                char readyToStartMessage[BUFF_SIZE];

                strcpy(readyToStartMessage, createReadyToStartMessage(playerId)); // 0x0a

                printf("check send message: %s\n", readyToStartMessage);

                bytes_sent = send(players[waitingPlayerId]->socket, readyToStartMessage, strlen(readyToStartMessage), 0);
                
            } else if (buff[0] == 0x06) {

                int roomId = extractStartGameMessage(buff);
                int knownPlayer = 0;

                for (int i = 0; i < number_of_players; i ++) {

                    if (players[i]->system_id == rooms[roomId].first_player_id) {

                        knownPlayer++;

                        char notifyStartGameMessage[BUFF_SIZE];
                        strcpy(notifyStartGameMessage, createNotifyStartMessage());

                        bytes_sent = send(players[i]->socket, notifyStartGameMessage, strlen(notifyStartGameMessage), 0);

                        if (knownPlayer == 2) {
                            break;
                        }
                    }
                }
            } else if (buff[0] == 0x07) {
                int roomId, playerId, direction;

                extractDirectionMessage(buff, &roomId, &playerId, &direction);

                if (rooms[roomId].first_player_id == playerId) {

                    for (int i = 0; i < number_of_players; i ++) {

                        if (players[i]->system_id == rooms[roomId].second_player_id) {
                            
                            char sdirection[BUFF_SIZE];

                            sprintf(sdirection, "%c%d", 0x08, direction);

                            bytes_sent = send(players[i]->socket, sdirection, strlen(sdirection), 0);

                            break;
                        }
                    }
                } else {
                     for (int i = 0; i < number_of_players; i ++) {
                        if (players[i]->system_id == rooms[roomId].first_player_id) {

                            char sdirection[BUFF_SIZE];

                            sprintf(sdirection, "%c%d", 0x08, direction);

                            bytes_sent = send(players[i]->socket, sdirection, strlen(sdirection), 0);

                            break;
                        }
                    }
                }
            } else if (buff[0] == 0x09) {

                int roomId, playerId, direction;
                extractDualShotMessage(buff, &roomId, &playerId, &direction);

                printf("check direction: %d %d %d\n", roomId, playerId, direction);

                if (rooms[roomId].first_player_id == playerId) {

                    for (int i = 0; i < number_of_players; i ++) {

                        if (players[i]->system_id == rooms[roomId].second_player_id) {

                            char sdirection[BUFF_SIZE];
                            sprintf(sdirection, "%c%d", 0x0c, direction);
                            bytes_sent = send(players[i]->socket, sdirection, strlen(sdirection), 0);
                            break;
                        }
                    }
                } else {
                     for (int i = 0; i < number_of_players; i ++) {

                        if (players[i]->system_id == rooms[roomId].first_player_id) {

                            char sdirection[BUFF_SIZE];
                            sprintf(sdirection, "%c%d", 0x0c, direction);
                            bytes_sent = send(players[i]->socket, sdirection, strlen(sdirection), 0);
                            break;
                        }
                    }
                }
            } else if (buff[0] == 0x0d) {
                int id, score;

                extractUpdateScoreMessage(buff, &id, &score);

                updateScore(player->list, id, score);
            } else if (buff[0] == 0x0e) {                
                node* root = player->list->root;

                int numPlayers = 0;
                node* current = root;
                node **playersArray = NULL;

                while (current != NULL) {
                    numPlayers++;
                    current = current->next;
                }

                playersArray = (struct node**)malloc(numPlayers * sizeof(node*));

                current = root;
                int i = 0;

                while (current != NULL) {
                    playersArray[i] = current;
                    current = current->next;
                    i++;
                }

                // Sort the array of player pointers by score in descending order
                qsort(playersArray, numPlayers, sizeof(node*), compare_scores);

                // Print the IDs and usernames of the top N players
                for (i = 0; i < 8 && i < numPlayers; i++) {
                    char usernameScoreMessage[BUFF_SIZE];

                    strcpy(usernameScoreMessage, createScoreMessage(playersArray[i]->element.username, playersArray[i]->element.score, i + 1));

                    bytes_sent = send(player->socket, usernameScoreMessage, strlen(usernameScoreMessage), 0);
                }

                free(playersArray);
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
    pthread_mutex_init(&room_nums_mutex, NULL);

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
        printf("check socket: %d\n", player->socket);

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
    for (int i = 1; i <= number_of_players; i++) {
        pthread_join(threadIDs[i], NULL);
    }

    close(server_sock);
    return 0;
}

void login(singleLinkedList* list,char  message[], int socket, int id){
    char *extractedUsername;
    char *extractedPassword;
    int bytes_sent;

    node* foundPlayer;
    extractUsernameAndPassword((unsigned char *)message, &extractedUsername, &extractedPassword);

    foundPlayer = findAccount(list, extractedUsername);

    if (foundPlayer != NULL) {
        
        if (foundPlayer->element.status == 1 && foundPlayer->element.logged == 0) {
            
            char sid[4];
            sprintf(sid,"%d*%d",foundPlayer->element.id, foundPlayer->element.score);
            players[id]->system_id = foundPlayer->element.id;

            if (strcmp(foundPlayer->element.password, extractedPassword) == 0) {
                bytes_sent = send(socket, sid, strlen(sid), 0);
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

void registerPlayer(singleLinkedList* list,char  message[], int socket, int id){

    char *extractedUsername;
    char *extractedPassword;
    int bytes_sent;

    extractUsernameAndPassword((unsigned char *)message, &extractedUsername, &extractedPassword);

    int res = registerUser(list, extractedUsername, extractedPassword, id);

    if (res == 0) {
        bytes_sent = send(socket, "existed", strlen("existed"), 0);
    } else {
        bytes_sent = send(socket, "success", strlen("success"), 0);
    }
}