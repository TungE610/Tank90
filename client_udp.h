#ifndef CLIENT_UDP_H
#define CLIENT_UDP_H

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>

void prepare_client(int *sock, struct sockaddr_in *client_addr);
void send_to_server(int sock, struct sockaddr_in serv_addr, int16_t id, int16_t keys);
int client_listen(int sock, int16_t *tab);

#endif