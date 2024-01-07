#ifndef MAPS_H
#define MAPS_H

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "textures.h"
#include "variables.h"

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

void renderSingleMap1(SDL_Renderer *renderer) {
    createSingleMap1(single_map_1);

    int mapX = (640 - (12 * 40)) / 2;
    int mapY = (480 - (12 * 40)) / 2;

    SDL_Rect borderRect = { mapX - 2, mapY - 2, 12 * 40 + 4, 12 * 40 + 4 };
    SDL_RenderFillRect(renderer, &borderRect);

    // Render the map
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 12; x++) {
            SDL_Rect destRect = { mapX + x * 40, mapY + y * 40, 40, 40 };
            SDL_Texture* texture = NULL;

            switch (single_map_1[y][x]) {
                case 0:
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
}

void updatePlayerPositionSingle(int *y, int *x, int deltaY, int deltaX) {
    single_map_1[*y][*x] = 0;
    *y += deltaY;
    *x += deltaX;

    single_map_1[*y][*x] = 9;
}

void moveDown(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (vertical_controller % stepEachBlock == 0 && *coor_y + 1 < 12 && single_map_1[*coor_y + 1][*coor_x] == 0) {
        if (horizontal_controller % stepEachBlock != 0 && horizontal_controller < *coor_x * stepEachBlock) {
            if (single_map_1[*coor_y + 1][*coor_x - 1] == 0 && single_map_1[*coor_y + 1][*coor_x] == 0) {
                ++vertical_controller;
                rect->y += stepSize;
            }
        } else if (horizontal_controller % stepEachBlock != 0 && horizontal_controller > *coor_x * stepEachBlock) {
            if (single_map_1[*coor_y + 1][*coor_x] == 0 && single_map_1[*coor_y + 1][*coor_x + 1] == 0) {
                ++vertical_controller;
                rect->y += stepSize;
            }
        } else if (horizontal_controller % stepEachBlock == 0) {
            ++vertical_controller;
            rect->y += stepSize;
        }
    } else if (vertical_controller % stepEachBlock == stepEachBlock -1  && vertical_controller + 1 == (*coor_y + 1) * stepEachBlock) {
        ++vertical_controller;
        rect->y += stepSize;
        updatePlayerPositionSingle(coor_y, coor_x, 1, 0);
    } else if (vertical_controller % stepEachBlock != 0) {
        ++vertical_controller;
        rect->y += stepSize;
    }

    *position_x = horizontal_controller;
    *position_y = vertical_controller;
}

void moveUp(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (vertical_controller % stepEachBlock == 0 && *coor_y - 1 >= 0 && single_map_1[*coor_y - 1][*coor_x] == 0) {
        if (horizontal_controller % stepEachBlock != 0 && horizontal_controller < *coor_x * stepEachBlock) {
            if (single_map_1[*coor_y - 1][*coor_x - 1] == 0 && single_map_1[*coor_y - 1][*coor_x] == 0) {
                --vertical_controller;
                rect->y -= stepSize;
            }
        } else if (horizontal_controller % stepEachBlock != 0 && horizontal_controller > *coor_x * stepEachBlock) {
            if (single_map_1[*coor_y - 1][*coor_x] == 0 && single_map_1[*coor_y - 1][*coor_x + 1] == 0) {
                --vertical_controller;
                rect->y -= stepSize;
            }
        } else if (horizontal_controller % stepEachBlock == 0) {
            --vertical_controller;
            rect->y -= stepSize;
        }
    } else if (vertical_controller % stepEachBlock == 1 && vertical_controller - 1 == (*coor_y - 1) * stepEachBlock) {
        --vertical_controller;
        rect->y -= stepSize;
        updatePlayerPositionSingle(coor_y, coor_x, -1, 0);
    } else if (vertical_controller % stepEachBlock != 0) {
        --vertical_controller;
        rect->y -= stepSize;
        
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void moveRight(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    // Check if moving right is possible and handle the cases
    if (horizontal_controller % stepEachBlock == 0 && *coor_x + 1 < 12 && single_map_1[*coor_y][*coor_x + 1] == 0) {
        if (vertical_controller % stepEachBlock != 0 && vertical_controller < *coor_y * stepEachBlock) {
            if (single_map_1[*coor_y][*coor_x + 1] == 0 && single_map_1[*coor_y - 1][*coor_x + 1] == 0) {
                ++horizontal_controller;
                rect->x += stepSize;
            }
        } else if (vertical_controller % stepEachBlock != 0 && vertical_controller > *coor_y * stepEachBlock) {
            if (single_map_1[*coor_y + 1][*coor_x + 1] == 0 && single_map_1[*coor_y][*coor_x + 1] == 0) {
                ++horizontal_controller;
                rect->x += stepSize;
            }
        } else if (vertical_controller % stepEachBlock == 0) {
            ++horizontal_controller;
            rect->x += stepSize;
        }
    } else if (horizontal_controller % stepEachBlock == stepEachBlock -1 && horizontal_controller + 1 == (*coor_x + 1) * stepEachBlock) {
        ++horizontal_controller;
        rect->x += stepSize;
        updatePlayerPositionSingle(coor_y, coor_x, 0, 1);
    } else if (horizontal_controller % stepEachBlock != 0) {
        ++horizontal_controller;
        rect->x += stepSize;
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void moveLeft(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (horizontal_controller % stepEachBlock == 0 && *coor_x - 1 >= 0 && single_map_1[*coor_y][*coor_x - 1] == 0) {
        if (vertical_controller % stepEachBlock != 0 && vertical_controller < *coor_y * stepEachBlock) {
            if (single_map_1[*coor_y][*coor_x - 1] == 0 && single_map_1[*coor_y - 1][*coor_x - 1] == 0) {
                --horizontal_controller;
                rect->x -= stepSize;
            }
        } else if (vertical_controller % stepEachBlock != 0 && vertical_controller > *coor_y * stepEachBlock) {
            if (single_map_1[*coor_y][*coor_x - 1] == 0 && single_map_1[*coor_y + 1][*coor_x - 1] == 0) {
                --horizontal_controller;
                rect->x -= stepSize;
            }
        } else if (vertical_controller % stepEachBlock == 0) {
            --horizontal_controller;
            rect->x -= stepSize;
        }
    } else if (horizontal_controller % stepEachBlock == 1 && horizontal_controller - 1 == (*coor_x - 1) * stepEachBlock) {
        --horizontal_controller;
        rect->x -= stepSize;
        updatePlayerPositionSingle(coor_y, coor_x, 0, -1);
    } else if (horizontal_controller % stepEachBlock != 0) {
        --horizontal_controller;
        rect->x -= stepSize;
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

#endif