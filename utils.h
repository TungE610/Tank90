#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>

void extractUsernameAndPassword(unsigned char *message, char **username, char **password) {


    // Find the position of the '*' separator
    unsigned char *separator = strchr((char *)message + 1, '*');

    if (separator == NULL) {
        fprintf(stderr, "Invalid message format. Missing separator '*'.\n");
        *username = NULL;
        *password = NULL;
        return;
    }

    // Calculate the lengths of the username and password
    size_t usernameLength = separator - (message + 1);
    size_t passwordLength = strlen((char *)separator + 1);

    // Allocate memory for username and password
    *username = (char *)malloc(usernameLength + 1);
    *password = (char *)malloc(passwordLength + 1);

    // Copy username and password
    memcpy(*username, message + 2, usernameLength-1);
    (*username)[usernameLength] = '\0';

    memcpy(*password, separator + 2, passwordLength-1);
    (*password)[passwordLength] = '\0';
}

#endif