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
char* createCreateRoomMessage(int id);
char* createChooseRoomMessage();
char* createRoomStateMessage(int id, int state, int first_player_id, int second_player_id);
char* createJoinRoomMessage(int playerId, int roomId);
int extractJoinRoomMessage(const char* message, int* playerId, int* roomId);

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

char *createRegisterMessage(char username[], char password[]) {

    size_t usernameLength = strlen(username);
    size_t passwordLength = strlen(password);
    
    size_t totalLength = 1 + usernameLength + 1 + passwordLength; // 1 for 0x01, 1 for the separator '*'

    // Allocate memory for the message
    unsigned char *message = (unsigned char *)malloc(totalLength);

    // Construct the message
    message[0] = 0x02;
    memcpy(message + 1, username, usernameLength);
    message[1 + usernameLength] = '*'; // Use '*' as a separator
    memcpy(message +1 + usernameLength + 1, password, passwordLength);

    return message;
}

char *createCreateRoomMessage(int id) {

    size_t totalLength;
    char sid[3]; // Increase the size of sid to accommodate up to 3 characters
    if (id < 10) {
        totalLength = 2; // Remove the "size_t" type declaration
        sprintf(sid, "%d", id);
        sid[1] = '\0';
    } else if (id >= 10 && id <= 99) {
        totalLength = 3; // Remove the "size_t" type declaration
        sprintf(sid, "%d", id);
    }

    // Allocate memory for the message
    unsigned char *message = (unsigned char *)malloc(totalLength + 1); // +1 for the null terminator
    // Check if memory allocation was successful
    if (message == NULL) {
        // Handle memory allocation failure here
        return NULL;
    }

    // Construct the message
    message[0] = 0x03;
    memcpy(message + 1, sid, strlen(sid));
    message[totalLength] = '\0'; // Null-terminate the message

    return (char *)message; // Cast the pointer to char*
}

char *createChooseRoomMessage() {
    unsigned char *message = (unsigned char *)malloc(1);

    message[0] = 0x04;

    return message;
}

char* createRoomStateMessage(int id, int state, int first_player_id, int second_player_id) {
    // Calculate the length of the message string based on state
    int len;
    if (state == 1) {
        len = snprintf(NULL, 0, "%d*%d*%d*0", id, state, first_player_id);
    } else if (state == 2) {
        len = snprintf(NULL, 0, "%d*%d*%d*%d", id, state, first_player_id, second_player_id);
    } else {
        // Handle an invalid state or other cases
        return NULL;
    }

    // Allocate memory for the message string
    char* message = (char*)malloc((len + 1) * sizeof(char));

    // Check if memory allocation was successful
    if (message != NULL) {
        // Format the message string based on state
        if (state == 1) {
            snprintf(message, len + 1, "%d*%d*%d*0", id, state, first_player_id);
        } else if (state == 2) {
            snprintf(message, len + 1, "%d*%d*%d*%d", id, state, first_player_id, second_player_id);
        }
    }

    return message;
}

char* createJoinRoomMessage(int playerId, int roomId) {
    char message[64]; // Assuming a maximum length for the message
    
    // Format the message string
    snprintf(message, sizeof(message), "%c%d*%d", 0x05, playerId, roomId);

    // Allocate memory for the string and copy the formatted message
    char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
    if (result != NULL) {
        strcpy(result, message);
    }

    return result;
}

char* createStartGameMessage(int roomId) {
    char message[64]; // Assuming a maximum length for the message
    
    // Format the message string
    snprintf(message, sizeof(message), "%c%d", 0x06, roomId);

    // Allocate memory for the string and copy the formatted message
    char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
    if (result != NULL) {
        strcpy(result, message);
    }

    return result;
}

char* createDirectionMessage(int roomId, int playerId, int direction) {
    char message[64]; // Assuming a maximum length for the message
    
    // Format the message string
    snprintf(message, sizeof(message), "%c%d*%d*%d", 0x07, roomId, playerId, direction);

    // Allocate memory for the string and copy the formatted message
    char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
    if (result != NULL) {
        strcpy(result, message);
    }

    return result;
}

char *createDualShotMessage(int roomId, int playerId, int direction) {
    char message[64]; // Assuming a maximum length for the message
    
    // Format the message string
    snprintf(message, sizeof(message), "%c%d*%d*%d", 0x09, roomId, playerId, direction);

    // Allocate memory for the string and copy the formatted message
    char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
    if (result != NULL) {
        strcpy(result, message);
    }

    return result;
}

int extractDualShotMessage(const char* message, int *roomId, int *playerId, int *direction) {
    if (message[0] != 0x09) {
        return 0; // Invalid message format
    }

    // Use sscanf to parse the player and room IDs
    if (sscanf(message + 1, "%d*%d*%d", roomId, playerId, direction) != 3) {
        return 0; // Parsing failed
    }

    return 1; // Extraction successful
}

int extractDirectionMessage(const char* message, int *roomId, int *playerId, int *direction) {
    if (message[0] != 0x07) {
        return 0; // Invalid message format
    }

    // Use sscanf to parse the player and room IDs
    if (sscanf(message + 1, "%d*%d*%d", roomId, playerId, direction) != 3) {
        return 0; // Parsing failed
    }

    return 1; // Extraction successful
}

int extractStartGameMessage(const char* message) {
    if (message == NULL || message[0] != 0x06) {
        // Invalid message or message format
        return -1; // You can return a specific error code or value to indicate failure.
    }

    int roomId;
    sscanf(message + 1, "%d", &roomId); // Skip the first byte (0x05) and read the integer.

    return roomId;
}

int extractJoinRoomMessage(const char* message, int* playerId, int* roomId) {
    // Check if the message starts with the expected format
    if (message[0] != 0x05) {
        return 0; // Invalid message format
    }

    // Use sscanf to parse the player and room IDs
    if (sscanf(message + 1, "%d*%d", playerId, roomId) != 2) {
        return 0; // Parsing failed
    }

    return 1; // Extraction successful
}

char *createReadyToStartMessage(int friendId) {
    char message[64]; // Assuming a maximum length for the message
    
    // Format the message string
    snprintf(message, sizeof(message), "%c%d", 0x0a, friendId);

    // Allocate memory for the string and copy the formatted message
    char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
    if (result != NULL) {
        strcpy(result, message);
    }

    return result;
}

int extractReadyToStartMessage(const char* message, int* friendId) {
    // Check if the message starts with the expected format
    if (message[0] != 0x0a) {
        return 0; // Invalid message format
    }

    // Use sscanf to parse the player and room IDs
    if (sscanf(message + 1, "%d", friendId) != 2) {
        return 0; // Parsing failed
    }

    return 1; // Extraction successful
}

char *createNotifyStartMessage() {
    unsigned char *message = (unsigned char *)malloc(1);

    message[0] = 0x0b;

    return message;
}

char *createUpdateScoreMessage(int id, int score) {
    char message[64]; // Assuming a maximum length for the message
    
    // Format the message string
    snprintf(message, sizeof(message), "%c%d*%d", 0x0d, id, score);

    // Allocate memory for the string and copy the formatted message
    char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
    if (result != NULL) {
        strcpy(result, message);
    }

    return result;
}

int extractUpdateScoreMessage(const char* message, int* id, int* score) {
    // Check if the message starts with the expected format
    if (message[0] != 0x0d) {
        return 0; // Invalid message format
    }

    // Use sscanf to parse the player and room IDs
    if (sscanf(message + 1, "%d*%d", id, score) != 2) {
        return 0; // Parsing failed
    }

    return 1; // Extraction successful
}

char* createScoreMessage(char username[], int score, int rank) {
    char message[64]; // Assuming a maximum length for the message
    
    // Format the message string
    snprintf(message, sizeof(message), "%c%s*%d*%d", 0x0f, username, score, rank);

    // Allocate memory for the string and copy the formatted message
    char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
    if (result != NULL) {
        strcpy(result, message);
    }

    return result;
}

char* createPauseMessage(int roomId,int playerId) {
    char message[64]; // Assuming a maximum length for the message
    
    // Format the message string
    snprintf(message, sizeof(message), "%c%d*%d", 0x10, roomId, playerId);

    // Allocate memory for the string and copy the formatted message
    char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
    if (result != NULL) {
        strcpy(result, message);
    }

    return result;
}

int extractPauseMessage(const char* message, int* roomId, int *playerId){
    if (message[0] != 0x10) {
        return 0; // Invalid message format
    }

    if (sscanf(message + 1, "%d*%d", roomId, playerId) != 2) {
        return 0; // Parsing failed
    }

    return 1; // Extraction successful
}

// char* createLeaveRoomMessage(char username[], int score, int rank) {
//     char message[64]; // Assuming a maximum length for the message
    
//     // Format the message string
//     snprintf(message, sizeof(message), "%c%s*%d*%d", 0x0f, username, score, rank);

//     // Allocate memory for the string and copy the formatted message
//     char* result = (char*)malloc(strlen(message) + 1); // +1 for the null terminator
//     if (result != NULL) {
//         strcpy(result, message);
//     }

//     return result;
// }

void extractScoreMessage(const char* message, char* username, int* score, int* rank) {
    // Check if the message starts with the expected header (0x0f)
    if (message[0] != 0x0f) {
        fprintf(stderr, "Invalid message header.\n");
        return;
    }

    // Use strtok to split the message into tokens based on '*' and '$' delimiters
    char* token;
    char* context;
    char temp_message[strlen(message) + 1]; // Make a copy of the message to avoid modifying the original
    strcpy(temp_message, message);

    token = strtok_r(temp_message + 1, "*", &context); // Skip the first character (0x0f)
    if (token == NULL) {
        fprintf(stderr, "Invalid message format.\n");
        return;
    }

    strncpy(username, token, 64);

    token = strtok_r(NULL, "*", &context);
    if (token == NULL) {
        fprintf(stderr, "Invalid message format.\n");
        return;
    }

    sscanf(token, "%d", score);

    token = strtok_r(NULL, "*", &context);
    if (token == NULL) {
        fprintf(stderr, "Invalid message format.\n");
        return;
    }

    sscanf(token, "%d", rank);
}


#endif