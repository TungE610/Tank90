#ifndef MAPS_H
#define MAPS_H

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>

int single_map_1[12][12];
int dual_map_1[12][12];

void createSingleMap1(int single_map_1[12][12]) {
    FILE* inputFile = fopen("map/single_stage_1_map.txt", "r");
    if (inputFile != NULL) {
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 12; x++) {
                fscanf(inputFile, "%d", &single_map_1[y][x]);
            }
        }
        fclose(inputFile);
    } else {
        fprintf(stderr, "Failed to open map.txt\n");
    }
}

void createDualMap1(int dual_map_1[12][12]) {
    FILE* inputFile = fopen("map/dual_stage_1_map.txt", "r");
    if (inputFile != NULL) {
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 12; x++) {
                fscanf(inputFile, "%d", &dual_map_1[y][x]);
            }
        }
        fclose(inputFile);
    } else {
        fprintf(stderr, "Failed to open map.txt\n");
    }
}

#endif