#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>

// uint16_t SUCCESS_LOGIN[] = {0x01, 0x01};
uint16_t NOTFOUND_LOGIN[] = {0x01, 0x02};
uint16_t LOCKED_LOGIN[] = {0x01, 0x03};
uint16_t LOGGED_LOGIN[] = {0x01, 0x04};
uint16_t INVALID_LOGIN[] = {0x01, 0x05};

char* createLoginMessage(char username[], char password[]);

char *createLoginMessage(char username[], char password[]) {

    size_t usernameLength = strlen(username);
    size_t passwordLength = strlen(password);
    
    size_t totalLength = 1 + usernameLength + 1 + passwordLength; // 1 for 0x01, 1 for the separator '*'

    // Allocate memory for the message
    unsigned char *message = (unsigned char *)malloc(totalLength);

    // Construct the message
    message[0] = 0x01;
    memcpy(message + 1, username, usernameLength);
    message[1 + usernameLength] = '*'; // Use '*' as a separator
    memcpy(message + 1 + usernameLength + 1, password, passwordLength);

    return message;
}

#endif