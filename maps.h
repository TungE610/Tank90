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
#include "utils.h"
#include <time.h>

int single_map_1[12][12];
int single_map_2[12][12];
int single_map_3[12][12];

int dual_map_1[12][12];

void singleShot(SDL_Texture *tank, SDL_Rect rect, SDL_Texture *meUp, SDL_Texture *meDown, SDL_Texture *meRight, SDL_Texture *meLeft, int player_shot) {
    for (int i =0; i < 20; i ++) {
        if (single_1_bullet[i].is_active == 0) {
            single_1_bullet[i].is_active = 1;

            if (player_shot == 1) {
                single_1_bullet[i].player_id = myId;
            } else {
                single_1_bullet[i].player_id = 0;
            }

            single_1_bulletRect[i].x = rect.x;
            single_1_bulletRect[i].y = rect.y;

            if (tank == meUp) {
                single_1_bullet[i].direction = UP;
                bulletTexture[i] = *(&bullet_up);
            } else if (tank == meDown) {
                single_1_bullet[i].direction = DOWN;
                bulletTexture[i] = *(&bullet_down);

            } else if (tank == meRight) {
                single_1_bullet[i].direction = RIGHT;
                bulletTexture[i] = *(&bullet_right);

            } else {
                single_1_bullet[i].direction = LEFT;
                bulletTexture[i] = *(&bullet_left);

            }
            break;
        }
    }
}

void singleShot2(SDL_Texture *tank, SDL_Rect rect, SDL_Texture *meUp, SDL_Texture *meDown, SDL_Texture *meRight, SDL_Texture *meLeft, int player_shot) {
    for (int i =0; i < 30; i ++) {
        if (single_2_bullet[i].is_active == 0) {
            single_2_bullet[i].is_active = 1;

            if (player_shot == 1) {
                single_2_bullet[i].player_id = myId;
            } else {
                single_2_bullet[i].player_id = 0;
            }

            single_2_bulletRect[i].x = rect.x;
            single_2_bulletRect[i].y = rect.y;

            if (tank == meUp) {
                single_2_bullet[i].direction = UP;
                bulletTexture[i] = *(&bullet_up);
            } else if (tank == meDown) {
                single_2_bullet[i].direction = DOWN;
                bulletTexture[i] = *(&bullet_down);

            } else if (tank == meRight) {
                single_2_bullet[i].direction = RIGHT;
                bulletTexture[i] = *(&bullet_right);

            } else {
                single_2_bullet[i].direction = LEFT;
                bulletTexture[i] = *(&bullet_left);

            }
            break;
        }
    }
}

void singleShot3(SDL_Texture *tank, SDL_Rect rect, SDL_Texture *meUp, SDL_Texture *meDown, SDL_Texture *meRight, SDL_Texture *meLeft, int player_shot) {
    for (int i =0; i < 30; i ++) {
        if (single_3_bullet[i].is_active == 0) {
            single_3_bullet[i].is_active = 1;

            if (player_shot == 1) {
                single_3_bullet[i].player_id = myId;
            } else {
                single_3_bullet[i].player_id = 0;
            }

            single_3_bulletRect[i].x = rect.x;
            single_3_bulletRect[i].y = rect.y;

            if (tank == meUp) {
                single_3_bullet[i].direction = UP;
                bulletTexture[i] = *(&bullet_up);
            } else if (tank == meDown) {
                single_3_bullet[i].direction = DOWN;
                bulletTexture[i] = *(&bullet_down);

            } else if (tank == meRight) {
                single_3_bullet[i].direction = RIGHT;
                bulletTexture[i] = *(&bullet_right);

            } else {
                single_3_bullet[i].direction = LEFT;
                bulletTexture[i] = *(&bullet_left);

            }
            break;
        }
    }
}

void createSingleMap1(int single_map[12][12]) {
    FILE* inputFile = fopen("map/single_stage_1_map.txt", "r");
    if (inputFile != NULL) {
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 12; x++) {
                fscanf(inputFile, "%d", &single_map[y][x]);
            }
        }
        fclose(inputFile);
    } else {
        fprintf(stderr, "Failed to open map.txt\n");
    }
}

void createSingleMap2(int single_map[12][12]) {
    FILE* inputFile = fopen("map/single_stage_2_map.txt", "r");
    if (inputFile != NULL) {
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 12; x++) {
                fscanf(inputFile, "%d", &single_map[y][x]);
            }
        }
        fclose(inputFile);
    } else {
        fprintf(stderr, "Failed to open map.txt\n");
    }
}

void createSingleMap3(int single_map[12][12]) {
    FILE* inputFile = fopen("map/single_stage_3_map.txt", "r");
    if (inputFile != NULL) {
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 12; x++) {
                fscanf(inputFile, "%d", &single_map[y][x]);
            }
        }
        fclose(inputFile);
    } else {
        fprintf(stderr, "Failed to open map.txt\n");
    }
}

void renderSingle(SDL_Renderer *renderer, int single_map[12][12], int map) {
    if (map == 1) {
        createSingleMap1(single_map);
    } else if (map == 2){
        createSingleMap2(single_map);
    } else if (map == 3) {
        createSingleMap3(single_map);
    }

    int mapX = (640 - (12 * 40)) / 2;
    int mapY = (480 - (12 * 40)) / 2;

    SDL_Rect borderRect = { mapX - 2, mapY - 2, 12 * 40 + 4, 12 * 40 + 4 };
    SDL_RenderFillRect(renderer, &borderRect);

    // Render the map
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 12; x++) {
            SDL_Rect destRect = { mapX + x * 40, mapY + y * 40, 40, 40 };
            SDL_Texture* texture = NULL;

            switch (single_map[y][x]) {
                case 0:
                    break;
                case 1:
                    texture = block;
                    break;
                case 2:
                    texture = iron;
                    break;
                case 3: 
                    texture = grass;
                    break;
                case 4: 
                    texture = water;
                    break;
                case 9:
                    texture = myTank;
                    break;
                default:
                    break;
            }

            if (texture == meUp || texture == meDown || texture == meRight || texture == meLeft) {
                if (map == 1) {
                    SDL_RenderCopy(renderer, texture, NULL, &single_controlRect);
                } else if (map == 2) {
                    SDL_RenderCopy(renderer, texture, NULL, &single_controlRect_2);
                } else if (map == 3) {
                    SDL_RenderCopy(renderer, texture, NULL, &single_controlRect_3);
                }
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

void updatePlayerPositionDual(int *y, int *x, int deltaY, int deltaX) {
    single_map_1[*y][*x] = 0;
    *y += deltaY;
    *x += deltaX;

    if (isFirstUserInRoom == 1) {
        single_map_1[*y][*x] = 9;
    } else {
        single_map_1[*y][*x] = 8;
    }
}


void moveDown(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize, int single_map[12][12]) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (vertical_controller % stepEachBlock == 0 && *coor_y + 1 < 12 && single_map[*coor_y + 1][*coor_x] == 0) {
        if (horizontal_controller % stepEachBlock != 0 && horizontal_controller < *coor_x * stepEachBlock) {
            if (single_map[*coor_y + 1][*coor_x - 1] == 0 && single_map[*coor_y + 1][*coor_x] == 0) {
                ++vertical_controller;
                rect->y += stepSize;
            }
        } else if (horizontal_controller % stepEachBlock != 0 && horizontal_controller > *coor_x * stepEachBlock) {
            if (single_map[*coor_y + 1][*coor_x] == 0 && single_map[*coor_y + 1][*coor_x + 1] == 0) {
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

void moveUp(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize, int single_map[12][12]) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (vertical_controller % stepEachBlock == 0 && *coor_y - 1 >= 0 && single_map[*coor_y - 1][*coor_x] == 0) {
        if (horizontal_controller % stepEachBlock != 0 && horizontal_controller < *coor_x * stepEachBlock) {
            if (single_map[*coor_y - 1][*coor_x - 1] == 0 && single_map[*coor_y - 1][*coor_x] == 0) {
                --vertical_controller;
                rect->y -= stepSize;
            }
        } else if (horizontal_controller % stepEachBlock != 0 && horizontal_controller > *coor_x * stepEachBlock) {
            if (single_map[*coor_y - 1][*coor_x] == 0 && single_map[*coor_y - 1][*coor_x + 1] == 0) {
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

void moveRight(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize, int single_map[12][12]) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    // Check if moving right is possible and handle the cases
    if (horizontal_controller % stepEachBlock == 0 && *coor_x + 1 < 12 && single_map[*coor_y][*coor_x + 1] == 0) {
        if (vertical_controller % stepEachBlock != 0 && vertical_controller < *coor_y * stepEachBlock) {
            if (single_map[*coor_y][*coor_x + 1] == 0 && single_map[*coor_y - 1][*coor_x + 1] == 0) {
                ++horizontal_controller;
                rect->x += stepSize;
            }
        } else if (vertical_controller % stepEachBlock != 0 && vertical_controller > *coor_y * stepEachBlock) {
            if (single_map[*coor_y + 1][*coor_x + 1] == 0 && single_map[*coor_y][*coor_x + 1] == 0) {
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

void moveLeft(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize, int single_map[12][12]) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (horizontal_controller % stepEachBlock == 0 && *coor_x - 1 >= 0 && single_map[*coor_y][*coor_x - 1] == 0) {
        if (vertical_controller % stepEachBlock != 0 && vertical_controller < *coor_y * stepEachBlock) {
            if (single_map[*coor_y][*coor_x - 1] == 0 && single_map[*coor_y - 1][*coor_x - 1] == 0) {
                --horizontal_controller;
                rect->x -= stepSize;
            }
        } else if (vertical_controller % stepEachBlock != 0 && vertical_controller > *coor_y * stepEachBlock) {
            if (single_map[*coor_y][*coor_x - 1] == 0 && single_map[*coor_y + 1][*coor_x - 1] == 0) {
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

void renderSingle1Enermies(SDL_Renderer *renderer, Enermy single_enermies[]) {
    int end = 1;
    for (int i = 0; i < 4; i++) {
        if (single_enermies[i].exist == 1){
            end = 0;
            SDL_RenderCopy(renderer, single_1_enermyTexture[i], NULL, &single_1_enermy_react[i]);
                if (single_1_consecutive_count[i] < 40) {

                single_1_direction[i] = single_1_previousDirection[i];
                single_1_consecutive_count[i]++;
            } else {
                do {
                    single_1_direction[i] = rand() % 4;
                } while (single_1_direction[i] == single_1_previousDirection[i]);

                single_1_consecutive_count[i] = 0; // Reset consecutive_count
                if ( i == 1) {
                    singleShot(single_1_enermyTexture[i], single_1_enermy_react[i], enermy_1_up, enermy_1_down, enermy_1_right, enermy_1_left, 0);
                } else if (i == 2) {
                    singleShot(single_1_enermyTexture[i], single_1_enermy_react[i], enermy_2_up, enermy_2_down, enermy_2_right, enermy_2_left, 0);
                } else if (i == 3) {
                    singleShot(single_1_enermyTexture[i], single_1_enermy_react[i], enermy_3_up, enermy_3_down, enermy_3_right, enermy_3_left, 0);
                } else {
                    singleShot(single_1_enermyTexture[i], single_1_enermy_react[i], enermy_4_up, enermy_4_down, enermy_4_right, enermy_4_left, 0);
                }
            }

            single_1_previousDirection[i] = single_1_direction[i];
        //                             // Update the enermyRect based on the chosen direction
            switch (single_1_direction[i]) {
                case 0:    
                    moveDown(&single_1_enermy_hozirontal_controller[i], &single_1_enermy_vertical_controller[i], &single_1_mode_enermy_postion_x[i], &single_1_mode_enermy_postion_y[i] , &single_1_enermy_react[i], 40, 1, single_map_1);
                    if ( i == 1) {
                        single_1_enermyTexture[i] = enermy_1_down;
                    } else if ( i == 2) {
                        single_1_enermyTexture[i] = enermy_2_down;
                    } else if ( i == 3) {
                        single_1_enermyTexture[i] = enermy_3_down;
                    } else {
                        single_1_enermyTexture[i] = enermy_4_down;
                    }
                    break;
                case 1: // Move down
                    moveUp(&single_1_enermy_hozirontal_controller[i], &single_1_enermy_vertical_controller[i], &single_1_mode_enermy_postion_x[i], &single_1_mode_enermy_postion_y[i], &single_1_enermy_react[i], 40, 1, single_map_1);
                    if ( i == 1) {
                        single_1_enermyTexture[i] = enermy_1_up;
                    } else if ( i == 2) {
                        single_1_enermyTexture[i] = enermy_2_up;
                    } else if ( i == 3) {
                        single_1_enermyTexture[i] = enermy_3_up;
                    } else {
                        single_1_enermyTexture[i] = enermy_4_up;
                    }
                    break;
                case 2: // Move left
                    moveLeft(&single_1_enermy_hozirontal_controller[i], &single_1_enermy_vertical_controller[i], &single_1_mode_enermy_postion_x[i], &single_1_mode_enermy_postion_y[i], &single_1_enermy_react[i], 40, 1, single_map_1);
                    if ( i == 1) {
                        single_1_enermyTexture[i] = enermy_1_left;
                    } else if ( i == 2) {
                        single_1_enermyTexture[i] = enermy_2_left;
                    } else if ( i == 3) {
                        single_1_enermyTexture[i] = enermy_3_left;
                    } else {
                        single_1_enermyTexture[i] = enermy_4_left;
                    }
                
                    break;
                case 3:
                    moveRight(&single_1_enermy_hozirontal_controller[i], &single_1_enermy_vertical_controller[i], &single_1_mode_enermy_postion_x[i], &single_1_mode_enermy_postion_y[i], &single_1_enermy_react[i], 40, 1, single_map_1);
                    if ( i == 1) {
                        single_1_enermyTexture[i] = enermy_1_right;
                    } else if ( i == 2) {
                        single_1_enermyTexture[i] = enermy_2_right;
                    } else if ( i == 3) {
                        single_1_enermyTexture[i] = enermy_3_right;
                    } else {
                        single_1_enermyTexture[i] = enermy_4_right;
                    }
                    break;
                default:
                    break;
            }
        }
        SDL_Delay(10);
    }

    if (end == 1) {
        state = CHANGING_TO_SINGLE_MAP_2;
    }
}

void renderBulletSingle1(SDL_Renderer *renderer) {
    for (int i = 0; i < 20; i ++) {
        if (single_1_bullet[i].is_active == 1) {
            SDL_RenderCopy(renderer, bulletTexture[i], NULL, &single_1_bulletRect[i]);
        }
        if (single_1_bullet[i].is_active == 1) {
            if (single_1_bullet[i].direction == UP) {
                for (int j = 0; j < 4; j++) {
                    int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum; k++) {
                        if (j == removed[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    } else {
                        if (single_1_bullet[i].player_id == myId && single_1_bulletRect[i].y >= single_1_enermy_react[j].y && single_1_enermy_react[j].y >= single_1_bulletRect[i].y - 8
                            && single_1_bulletRect[i].x > single_1_enermy_react[j].x - 20 && single_1_bulletRect[i].x < single_1_enermy_react[j].x + 20 && myId != 0
                        ) {
                            single_1_enermies[j].blood--;
                            if (single_1_enermies[j].blood <= 0) {
                                single_scores++;

                                single_1_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                                SDL_RenderCopy(renderer, single_1_enermyTexture[j], NULL, &single_1_enermy_react[j]);
                                SDL_DestroyTexture(single_1_enermyTexture[j]);
                                single_1_enermies[j].exist = 0;
                                
                                removed[removeNum] = j;
                                removeNum++;
                            }

                            single_1_bullet[i].is_active = 0;
                            single_1_bullet[i].player_id = 0;
                            break;
                        }
                    }
                }

                if (single_1_bulletRect[i].y < 8 || (single_map_1[(single_1_bulletRect[i].y)/40 - 1][round_integer_division(single_1_bulletRect[i].x-80, 40)] != 0 && single_1_bulletRect[i].y % 40 < 15)) {
                    single_1_bullet[i].is_active = 0;
                    single_1_bullet[i].player_id = 0;
                }

                if (single_1_bullet[i].is_active == 1) {
                    single_1_bulletRect[i].y -= 8;
                }
            } else if (single_1_bullet[i].direction == DOWN) {
                for (int j = 0 ; j < 4; j ++) {
                     int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum; k++) {
                        if (j == removed[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_1_bullet[i].player_id == myId && single_1_bulletRect[i].y <= single_1_enermy_react[j].y && single_1_enermy_react[j].y <= single_1_bulletRect[i].y + 8
                        && single_1_bulletRect[i].x > single_1_enermy_react[j].x - 20 && single_1_bulletRect[i].x < single_1_enermy_react[j].x + 20 && myId != 0
                    ) {

                        single_1_enermies[j].blood --;
                        single_1_bullet[i].player_id = 0;
                        single_1_bullet[i].is_active = 0;

                        if (single_1_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_1_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_1_enermyTexture[j], NULL, &single_1_enermy_react[j]);
                            SDL_DestroyTexture(single_1_enermyTexture[j]);
                            single_1_enermies[j].exist = 0;

                            removed[removeNum] = j;
                            removeNum++;
                        }
                    }
                }
                if (single_1_bulletRect[i].y > 440 || (single_map_1[(single_1_bulletRect[i].y)/40 + 2][round_integer_division(single_1_bulletRect[i].x-80, 40)] != 0 && single_1_bulletRect[i].y % 40 > 15)) {
                    single_1_bullet[i].is_active = 0;
                    single_1_bullet[i].player_id = 0;
                }
                
                if (single_1_bullet[i].is_active == 1) {
                    single_1_bulletRect[i].y += 8;
                }
            } else if (single_1_bullet[i].direction == RIGHT) {
                for (int j = 0 ; j < 4; j ++) {
                     int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum; k++) {
                        if (j == removed[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_1_bullet[i].player_id == myId && single_1_bulletRect[i].x <= single_1_enermy_react[j].x  && single_1_enermy_react[j].x <= single_1_bulletRect[i].x + 8
                        && single_1_bulletRect[i].y > single_1_enermy_react[j].y - 20 && single_1_bulletRect[i].y < single_1_enermy_react[j].y + 20 && myId != 0
                    ) {

                        single_1_enermies[j].blood --;
                        single_1_bullet[i].player_id = 0;
                        single_1_bullet[i].is_active = 0;

                        if (single_1_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_1_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_1_enermyTexture[j], NULL, &single_1_enermy_react[j]);
                            SDL_DestroyTexture(single_1_enermyTexture[j]);
                            single_1_enermies[j].exist = 0;

                            removed[removeNum] = j;
                            removeNum++;
                        }
                    }
                }
                if (single_1_bulletRect[i].x > 520 || (single_map_1[round_integer_division(single_1_bulletRect[i].y,40)][(single_1_bulletRect[i].x-80)/40 + 2] != 0 && single_1_bulletRect[i].x % 40 > 15)) {
                    single_1_bullet[i].is_active = 0;
                    single_1_bullet[i].player_id = 0;
                } 

                if (single_1_bullet[i].is_active == 1) {
                    single_1_bulletRect[i].x += 8;
                }
            } else {
                for (int j = 0 ; j < 4; j ++) {
                    int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum; k++) {
                        if (j == removed[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_1_bullet[i].player_id == myId && single_1_bulletRect[i].x >= single_1_enermy_react[j].x && single_1_enermy_react[j].x >= single_1_bulletRect[i].x - 8
                        && single_1_bulletRect[i].y > single_1_enermy_react[j].y - 20 && single_1_bulletRect[i].y < single_1_enermy_react[j].y + 20 && myId != 0
                    ) {

                        single_1_enermies[j].blood --;
                        single_1_bullet[i].player_id = 0;
                        single_1_bullet[i].is_active = 0;

                        if (single_1_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_1_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_1_enermyTexture[j], NULL, &single_1_enermy_react[j]);
                            SDL_DestroyTexture(single_1_enermyTexture[j]);
                            single_1_enermies[j].exist = 0;

                            removed[removeNum] = j;
                            removeNum++;
                        }
                    }
                }
                if (single_1_bulletRect[i].x < 80 || (single_map_1[round_integer_division(single_1_bulletRect[i].y,40)][(single_1_bulletRect[i].x-80)/40 - 1] != 0 && single_1_bulletRect[i].x % 40 < 15)) {
                    single_1_bullet[i].is_active = 0;
                    single_1_bullet[i].player_id = 0;
                } 
                if (single_1_bullet[i].is_active == 1) {
                    single_1_bulletRect[i].x -= 8;
                }
            }
        }
    }
}

void renderChangeMapScreen(SDL_Renderer *renderer, int newMap) {
    char snewMap[BUFF_SIZE], renderedString[BUFF_SIZE] = "Stage ";
    sprintf(snewMap, "%d", newMap);
    strcat(renderedString, snewMap);

    SDL_Surface *newMapTextSurface = TTF_RenderText_Solid(MEDIUM_FONT, renderedString, WHITE);
    SDL_Texture *newMapTextTexture = SDL_CreateTextureFromSurface(renderer, newMapTextSurface);
    SDL_Rect newMapTextRenderQuad = { 250, 230, newMapTextSurface->w, newMapTextSurface->h };
    SDL_RenderCopy(renderer, newMapTextTexture, NULL, &newMapTextRenderQuad);
}

void renderSingle2Enermies(SDL_Renderer *renderer, Enermy single_enermies[]) {
    int end = 1;
    for (int i = 0; i < 5; i++) {
        if (single_enermies[i].exist == 1){
            end = 0;
            SDL_RenderCopy(renderer, single_2_enermyTexture[i], NULL, &single_2_enermy_react[i]);
                if (single_2_consecutive_count[i] < 40) {

                single_2_direction[i] = single_2_previousDirection[i];
                single_2_consecutive_count[i]++;
            } else {
                do {
                    single_2_direction[i] = rand() % 4;
                } while (single_2_direction[i] == single_2_previousDirection[i]);

                single_2_consecutive_count[i] = 0; // Reset consecutive_count
                if ( i == 1) {
                    singleShot2(single_2_enermyTexture[i], single_2_enermy_react[i], enermy_1_up, enermy_1_down, enermy_1_right, enermy_1_left, 0);
                } else if (i == 2) {
                    singleShot2(single_2_enermyTexture[i], single_2_enermy_react[i], enermy_2_up, enermy_2_down, enermy_2_right, enermy_2_left, 0);
                } else if (i == 3) {
                    singleShot2(single_2_enermyTexture[i], single_2_enermy_react[i], enermy_3_up, enermy_3_down, enermy_3_right, enermy_3_left, 0);
                } else if ( i == 4) {
                    singleShot2(single_2_enermyTexture[i], single_2_enermy_react[i], enermy_4_up, enermy_4_down, enermy_4_right, enermy_4_left, 0);
                } else {
                    singleShot2(single_2_enermyTexture[i], single_2_enermy_react[i], enermy_5_up, enermy_5_down, enermy_5_right, enermy_5_left, 0);
                }
            }

            single_2_previousDirection[i] = single_2_direction[i];
        //                             // Update the enermyRect based on the chosen direction
            switch (single_2_direction[i]) {
                case 0:    
                    moveDown(&single_2_enermy_hozirontal_controller[i], &single_2_enermy_vertical_controller[i], &single_2_mode_enermy_postion_x[i], &single_2_mode_enermy_postion_y[i] , &single_2_enermy_react[i], 40, 1, single_map_2);
                    if ( i == 1) {
                        single_2_enermyTexture[i] = enermy_1_down;
                    } else if ( i == 2) {
                        single_2_enermyTexture[i] = enermy_2_down;
                    } else if ( i == 3) {
                        single_2_enermyTexture[i] = enermy_3_down;
                    }  else if ( i == 4) {
                        single_2_enermyTexture[i] = enermy_4_down;
                    } else {
                        single_2_enermyTexture[i] = enermy_5_down;
                    }
                    break;
                case 1: // Move down
                    moveUp(&single_2_enermy_hozirontal_controller[i], &single_2_enermy_vertical_controller[i], &single_2_mode_enermy_postion_x[i], &single_2_mode_enermy_postion_y[i], &single_2_enermy_react[i], 40, 1, single_map_2);
                    if ( i == 1) {
                        single_2_enermyTexture[i] = enermy_1_up;
                    } else if ( i == 2) {
                        single_2_enermyTexture[i] = enermy_2_up;
                    } else if ( i == 3) {
                        single_2_enermyTexture[i] = enermy_3_up;
                    } else if ( i == 4) {
                        single_2_enermyTexture[i] = enermy_4_up;
                    } else {
                        single_2_enermyTexture[i] = enermy_5_up;
                    }
                    break;
                case 2: // Move left
                    moveLeft(&single_2_enermy_hozirontal_controller[i], &single_2_enermy_vertical_controller[i], &single_2_mode_enermy_postion_x[i], &single_2_mode_enermy_postion_y[i], &single_2_enermy_react[i], 40, 1, single_map_2);
                    if ( i == 1) {
                        single_2_enermyTexture[i] = enermy_1_left;
                    } else if ( i == 2) {
                        single_2_enermyTexture[i] = enermy_2_left;
                    } else if ( i == 3) {
                        single_2_enermyTexture[i] = enermy_3_left;
                    } else if ( i == 4) {
                        single_2_enermyTexture[i] = enermy_4_left;
                    } else {
                        single_2_enermyTexture[i] = enermy_5_left;
                    }
                
                    break;
                case 3:
                    moveRight(&single_2_enermy_hozirontal_controller[i], &single_2_enermy_vertical_controller[i], &single_2_mode_enermy_postion_x[i], &single_2_mode_enermy_postion_y[i], &single_2_enermy_react[i], 40, 1, single_map_2);
                    if ( i == 1) {
                        single_2_enermyTexture[i] = enermy_1_right;
                    } else if ( i == 2) {
                        single_2_enermyTexture[i] = enermy_2_right;
                    } else if ( i == 3) {
                        single_2_enermyTexture[i] = enermy_3_right;
                    } else if ( i == 4) {
                        single_2_enermyTexture[i] = enermy_4_right;
                    } else {
                        single_2_enermyTexture[i] = enermy_5_right;
                    }
                    break;
                default:
                    break;
            }
        }
        SDL_Delay(5);
    }

    if (end == 1) {
        state = CHANGING_TO_SINGLE_MAP_3;
    }
}

void renderBulletSingle2(SDL_Renderer *renderer) {
    for (int i = 0; i < 30; i ++) {
        if (single_2_bullet[i].is_active == 1) {
            SDL_RenderCopy(renderer, bulletTexture[i], NULL, &single_2_bulletRect[i]);
        }
        if (single_2_bullet[i].is_active == 1) {
            if (single_2_bullet[i].direction == UP) {
                for (int j = 0; j < 5; j++) {
                    int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum_2; k++) {
                        if (j == removed_2[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    } else {
                        if (single_2_bullet[i].player_id == myId && single_2_bulletRect[i].y >= single_2_enermy_react[j].y && single_2_enermy_react[j].y >= single_2_bulletRect[i].y - 8
                            && single_2_bulletRect[i].x > single_2_enermy_react[j].x - 20 && single_2_bulletRect[i].x < single_2_enermy_react[j].x + 20 && myId != 0
                        ) {
                            single_2_enermies[j].blood--;
                            if (single_2_enermies[j].blood <= 0) {
                                single_scores++;

                                single_2_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                                SDL_RenderCopy(renderer, single_2_enermyTexture[j], NULL, &single_2_enermy_react[j]);
                                SDL_DestroyTexture(single_2_enermyTexture[j]);
                                single_2_enermies[j].exist = 0;
                                
                                removed_2[removeNum_2] = j;
                                removeNum_2++;
                            }

                            single_2_bullet[i].is_active = 0;
                            single_2_bullet[i].player_id = 0;
                            break;
                        }
                    }
                }

                if (single_2_bulletRect[i].y < 8 || (single_map_2[(single_2_bulletRect[i].y)/40 - 1][round_integer_division(single_2_bulletRect[i].x-80, 40)] != 0 && single_2_bulletRect[i].y % 40 < 15)) {
                    single_2_bullet[i].is_active = 0;
                    single_2_bullet[i].player_id = 0;
                }

                if (single_2_bullet[i].is_active == 1) {
                    single_2_bulletRect[i].y -= 8;
                }
            } else if (single_2_bullet[i].direction == DOWN) {
                for (int j = 0 ; j < 5; j ++) {
                     int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum_2; k++) {
                        if (j == removed_2[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_2_bullet[i].player_id == myId && single_2_bulletRect[i].y <= single_2_enermy_react[j].y && single_2_enermy_react[j].y <= single_2_bulletRect[i].y + 8
                        && single_2_bulletRect[i].x > single_2_enermy_react[j].x - 20 && single_2_bulletRect[i].x < single_2_enermy_react[j].x + 20 && myId != 0
                    ) {

                        single_2_enermies[j].blood --;
                        single_2_bullet[i].player_id = 0;
                        single_2_bullet[i].is_active = 0;

                        if (single_2_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_2_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_2_enermyTexture[j], NULL, &single_2_enermy_react[j]);
                            SDL_DestroyTexture(single_2_enermyTexture[j]);
                            single_2_enermies[j].exist = 0;

                            removed_2[removeNum_2] = j;
                            removeNum_2++;
                        }
                    }
                }
                if (single_2_bulletRect[i].y > 440 || (single_map_2[(single_2_bulletRect[i].y)/40 + 2][round_integer_division(single_2_bulletRect[i].x-80, 40)] != 0 && single_2_bulletRect[i].y % 40 > 15)) {
                    single_2_bullet[i].is_active = 0;
                    single_2_bullet[i].player_id = 0;
                }
                
                if (single_2_bullet[i].is_active == 1) {
                    single_2_bulletRect[i].y += 8;
                }
            } else if (single_2_bullet[i].direction == RIGHT) {
                for (int j = 0 ; j < 5; j ++) {
                     int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum_2; k++) {
                        if (j == removed_2[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_2_bullet[i].player_id == myId && single_2_bulletRect[i].x <= single_2_enermy_react[j].x  && single_2_enermy_react[j].x <= single_2_bulletRect[i].x + 8
                        && single_2_bulletRect[i].y > single_2_enermy_react[j].y - 20 && single_2_bulletRect[i].y < single_2_enermy_react[j].y + 20 && myId != 0
                    ) {

                        single_2_enermies[j].blood --;
                        single_2_bullet[i].player_id = 0;
                        single_2_bullet[i].is_active = 0;

                        if (single_2_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_2_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_2_enermyTexture[j], NULL, &single_2_enermy_react[j]);
                            SDL_DestroyTexture(single_2_enermyTexture[j]);
                            single_2_enermies[j].exist = 0;

                            removed_2[removeNum_2] = j;
                            removeNum_2++;
                        }
                    }
                }
                if (single_2_bulletRect[i].x > 520 || (single_map_2[round_integer_division(single_2_bulletRect[i].y,40)][(single_2_bulletRect[i].x-80)/40 + 2] != 0 && single_2_bulletRect[i].x % 40 > 15)) {
                    single_2_bullet[i].is_active = 0;
                    single_2_bullet[i].player_id = 0;
                } 

                if (single_2_bullet[i].is_active == 1) {
                    single_2_bulletRect[i].x += 8;
                }
            } else {
                for (int j = 0 ; j < 5; j ++) {
                    int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum_2; k++) {
                        if (j == removed_2[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_2_bullet[i].player_id == myId && single_2_bulletRect[i].x >= single_2_enermy_react[j].x && single_2_enermy_react[j].x >= single_2_bulletRect[i].x - 8
                        && single_2_bulletRect[i].y > single_2_enermy_react[j].y - 20 && single_2_bulletRect[i].y < single_2_enermy_react[j].y + 20 && myId != 0
                    ) {

                        single_2_enermies[j].blood --;
                        single_2_bullet[i].player_id = 0;
                        single_2_bullet[i].is_active = 0;

                        if (single_2_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_2_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_2_enermyTexture[j], NULL, &single_2_enermy_react[j]);
                            SDL_DestroyTexture(single_2_enermyTexture[j]);
                            single_2_enermies[j].exist = 0;

                            removed_2[removeNum_2] = j;
                            removeNum_2++;
                        }
                    }
                }
                if (single_2_bulletRect[i].x < 80 || (single_map_2[round_integer_division(single_2_bulletRect[i].y,40)][(single_2_bulletRect[i].x-80)/40 - 1] != 0 && single_2_bulletRect[i].x % 40 < 15)) {
                    single_2_bullet[i].is_active = 0;
                    single_2_bullet[i].player_id = 0;
                } 
                if (single_2_bullet[i].is_active == 1) {
                    single_2_bulletRect[i].x -= 8;
                }
            }
        }
    }
}

void renderSingle3Enermies(SDL_Renderer *renderer, Enermy single_enermies[]) {
    int end = 1;
    for (int i = 0; i < 6; i++) {
        if (single_enermies[i].exist == 1){
            end = 0;
            SDL_RenderCopy(renderer, single_3_enermyTexture[i], NULL, &single_3_enermy_react[i]);
                if (single_3_consecutive_count[i] < 40) {

                single_3_direction[i] = single_3_previousDirection[i];
                single_3_consecutive_count[i]++;
            } else {
                do {
                    single_3_direction[i] = rand() % 4;
                } while (single_3_direction[i] == single_3_previousDirection[i]);

                single_3_consecutive_count[i] = 0; // Reset consecutive_count
                if ( i == 1) {
                    singleShot3(single_3_enermyTexture[i], single_3_enermy_react[i], enermy_1_up, enermy_1_down, enermy_1_right, enermy_1_left, 0);
                } else if (i == 2) {
                    singleShot3(single_3_enermyTexture[i], single_3_enermy_react[i], enermy_2_up, enermy_2_down, enermy_2_right, enermy_2_left, 0);
                } else if (i == 3) {
                    singleShot3(single_3_enermyTexture[i], single_3_enermy_react[i], enermy_3_up, enermy_3_down, enermy_3_right, enermy_3_left, 0);
                } else if ( i == 4) {
                    singleShot3(single_3_enermyTexture[i], single_3_enermy_react[i], enermy_4_up, enermy_4_down, enermy_4_right, enermy_4_left, 0);
                } else if ( i == 5) {
                    singleShot3(single_3_enermyTexture[i], single_3_enermy_react[i], enermy_5_up, enermy_5_down, enermy_5_right, enermy_5_left, 0);
                } else {
                    singleShot3(single_3_enermyTexture[i], single_3_enermy_react[i], enermy_6_up, enermy_6_down, enermy_6_right, enermy_6_left, 0);
                }
            }

            single_3_previousDirection[i] = single_3_direction[i];
        //                             // Update the enermyRect based on the chosen direction
            switch (single_3_direction[i]) {
                case 0:    
                    moveDown(&single_3_enermy_hozirontal_controller[i], &single_3_enermy_vertical_controller[i], &single_3_mode_enermy_postion_x[i], &single_3_mode_enermy_postion_y[i] , &single_3_enermy_react[i], 40, 1, single_map_3);
                    if ( i == 1) {
                        single_3_enermyTexture[i] = enermy_1_down;
                    } else if ( i == 2) {
                        single_3_enermyTexture[i] = enermy_2_down;
                    } else if ( i == 3) {
                        single_3_enermyTexture[i] = enermy_3_down;
                    }  else if ( i == 4) {
                        single_3_enermyTexture[i] = enermy_4_down;
                    }  else if ( i == 5) {
                        single_3_enermyTexture[i] = enermy_5_down;
                    } else {
                        single_3_enermyTexture[i] = enermy_6_down;
                    }
                    break;
                case 1: // Move down
                    moveUp(&single_3_enermy_hozirontal_controller[i], &single_3_enermy_vertical_controller[i], &single_3_mode_enermy_postion_x[i], &single_3_mode_enermy_postion_y[i], &single_3_enermy_react[i], 40, 1, single_map_3);
                    if ( i == 1) {
                        single_3_enermyTexture[i] = enermy_1_up;
                    } else if ( i == 2) {
                        single_3_enermyTexture[i] = enermy_2_up;
                    } else if ( i == 3) {
                        single_3_enermyTexture[i] = enermy_3_up;
                    } else if ( i == 4) {
                        single_3_enermyTexture[i] = enermy_4_up;
                    } else if ( i == 5) {
                        single_3_enermyTexture[i] = enermy_5_up;
                    } else {
                        single_3_enermyTexture[i] = enermy_6_up;
                    }
                    break;
                case 2: // Move left
                    moveLeft(&single_3_enermy_hozirontal_controller[i], &single_3_enermy_vertical_controller[i], &single_3_mode_enermy_postion_x[i], &single_3_mode_enermy_postion_y[i], &single_3_enermy_react[i], 40, 1, single_map_3);
                    if ( i == 1) {
                        single_3_enermyTexture[i] = enermy_1_left;
                    } else if ( i == 2) {
                        single_3_enermyTexture[i] = enermy_2_left;
                    } else if ( i == 3) {
                        single_3_enermyTexture[i] = enermy_3_left;
                    } else if ( i == 4) {
                        single_3_enermyTexture[i] = enermy_4_left;
                    } else if ( i == 5) {
                        single_3_enermyTexture[i] = enermy_5_left;
                    } else {
                        single_3_enermyTexture[i] = enermy_6_left;
                    }
                
                    break;
                case 3:
                    moveRight(&single_3_enermy_hozirontal_controller[i], &single_3_enermy_vertical_controller[i], &single_3_mode_enermy_postion_x[i], &single_3_mode_enermy_postion_y[i], &single_3_enermy_react[i], 40, 1, single_map_3);
                    if ( i == 1) {
                        single_3_enermyTexture[i] = enermy_1_right;
                    } else if ( i == 2) {
                        single_3_enermyTexture[i] = enermy_2_right;
                    } else if ( i == 3) {
                        single_3_enermyTexture[i] = enermy_3_right;
                    } else if ( i == 4) {
                        single_3_enermyTexture[i] = enermy_4_right;
                    } else if ( i == 5) {
                        single_3_enermyTexture[i] = enermy_5_right;
                    } else {
                        single_3_enermyTexture[i] = enermy_6_right;
                    }
                    break;
                default:
                    break;
            }
        }
        SDL_Delay(3);
    }

    if (end == 1) {
        state = CHOOSE_MODE;
    }
}

void renderBulletSingle3(SDL_Renderer *renderer) {
    for (int i = 0; i < 30; i ++) {
        if (single_3_bullet[i].is_active == 1) {
            SDL_RenderCopy(renderer, bulletTexture[i], NULL, &single_3_bulletRect[i]);
        }
        if (single_3_bullet[i].is_active == 1) {
            if (single_3_bullet[i].direction == UP) {
                for (int j = 0; j < 6; j++) {
                    int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum_3; k++) {
                        if (j == removed_3[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    } else {
                        if (single_3_bullet[i].player_id == myId && single_3_bulletRect[i].y >= single_3_enermy_react[j].y && single_3_enermy_react[j].y >= single_3_bulletRect[i].y - 8
                            && single_3_bulletRect[i].x > single_3_enermy_react[j].x - 20 && single_3_bulletRect[i].x < single_3_enermy_react[j].x + 20 && myId != 0
                        ) {
                            single_3_enermies[j].blood--;
                            if (single_3_enermies[j].blood <= 0) {
                                single_scores++;

                                single_3_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                                SDL_RenderCopy(renderer, single_3_enermyTexture[j], NULL, &single_3_enermy_react[j]);
                                SDL_DestroyTexture(single_3_enermyTexture[j]);
                                single_3_enermies[j].exist = 0;
                                
                                removed_3[removeNum_3] = j;
                                removeNum_3++;
                            }

                            single_3_bullet[i].is_active = 0;
                            single_3_bullet[i].player_id = 0;
                            break;
                        }
                    }
                }

                if (single_3_bulletRect[i].y < 8 || (single_map_3[(single_3_bulletRect[i].y)/40 - 1][round_integer_division(single_3_bulletRect[i].x-80, 40)] != 0 && single_3_bulletRect[i].y % 40 < 15)) {
                    single_3_bullet[i].is_active = 0;
                    single_3_bullet[i].player_id = 0;
                }

                if (single_3_bullet[i].is_active == 1) {
                    single_3_bulletRect[i].y -= 8;
                }
            } else if (single_3_bullet[i].direction == DOWN) {
                for (int j = 0 ; j < 6; j ++) {
                     int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum_3; k++) {
                        if (j == removed_3[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_3_bullet[i].player_id == myId && single_3_bulletRect[i].y <= single_3_enermy_react[j].y && single_3_enermy_react[j].y <= single_3_bulletRect[i].y + 8
                        && single_3_bulletRect[i].x > single_3_enermy_react[j].x - 20 && single_3_bulletRect[i].x < single_3_enermy_react[j].x + 20 && myId != 0
                    ) {

                        single_3_enermies[j].blood --;
                        single_3_bullet[i].player_id = 0;
                        single_3_bullet[i].is_active = 0;

                        if (single_3_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_3_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_3_enermyTexture[j], NULL, &single_3_enermy_react[j]);
                            SDL_DestroyTexture(single_3_enermyTexture[j]);
                            single_3_enermies[j].exist = 0;

                            removed_3[removeNum_3] = j;
                            removeNum_3++;
                        }
                    }
                }
                if (single_3_bulletRect[i].y > 440 || (single_map_3[(single_3_bulletRect[i].y)/40 + 2][round_integer_division(single_3_bulletRect[i].x-80, 40)] != 0 && single_3_bulletRect[i].y % 40 > 15)) {
                    single_3_bullet[i].is_active = 0;
                    single_3_bullet[i].player_id = 0;
                }
                
                if (single_3_bullet[i].is_active == 1) {
                    single_3_bulletRect[i].y += 8;
                }
            } else if (single_3_bullet[i].direction == RIGHT) {
                for (int j = 0 ; j < 6; j ++) {
                     int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum_3; k++) {
                        if (j == removed_3[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_3_bullet[i].player_id == myId && single_3_bulletRect[i].x <= single_3_enermy_react[j].x  && single_3_enermy_react[j].x <= single_3_bulletRect[i].x + 8
                        && single_3_bulletRect[i].y > single_3_enermy_react[j].y - 20 && single_3_bulletRect[i].y < single_3_enermy_react[j].y + 20 && myId != 0
                    ) {

                        single_3_enermies[j].blood --;
                        single_3_bullet[i].player_id = 0;
                        single_3_bullet[i].is_active = 0;

                        if (single_3_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_3_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_3_enermyTexture[j], NULL, &single_3_enermy_react[j]);
                            SDL_DestroyTexture(single_3_enermyTexture[j]);
                            single_3_enermies[j].exist = 0;

                            removed_3[removeNum_3] = j;
                            removeNum_3++;
                        }
                    }
                }
                if (single_3_bulletRect[i].x > 520 || (single_map_3[round_integer_division(single_3_bulletRect[i].y,40)][(single_3_bulletRect[i].x-80)/40 + 2] != 0 && single_3_bulletRect[i].x % 40 > 15)) {
                    single_3_bullet[i].is_active = 0;
                    single_3_bullet[i].player_id = 0;
                } 

                if (single_3_bullet[i].is_active == 1) {
                    single_3_bulletRect[i].x += 8;
                }
            } else {
                for (int j = 0 ; j < 6; j ++) {
                    int alreadyRemoved = 0;
                    for (int k = 0; k < removeNum_3; k++) {
                        if (j == removed_3[k]) {
                            alreadyRemoved = 1;
                            break;
                        }
                    }
                    if (alreadyRemoved) {
                        continue;
                    }
                    if (single_3_bullet[i].player_id == myId && single_3_bulletRect[i].x >= single_3_enermy_react[j].x && single_3_enermy_react[j].x >= single_3_bulletRect[i].x - 8
                        && single_3_bulletRect[i].y > single_3_enermy_react[j].y - 20 && single_3_bulletRect[i].y < single_3_enermy_react[j].y + 20 && myId != 0
                    ) {

                        single_3_enermies[j].blood --;
                        single_3_bullet[i].player_id = 0;
                        single_3_bullet[i].is_active = 0;

                        if (single_3_enermies[j].blood <= 0) {
                            single_scores ++;
                            single_3_enermyTexture[j] = IMG_LoadTexture(renderer, "images/hitted.png");
                            SDL_RenderCopy(renderer, single_3_enermyTexture[j], NULL, &single_3_enermy_react[j]);
                            SDL_DestroyTexture(single_3_enermyTexture[j]);
                            single_3_enermies[j].exist = 0;

                            removed_3[removeNum_3] = j;
                            removeNum_3++;
                        }
                    }
                }
                if (single_3_bulletRect[i].x < 80 || (single_map_3[round_integer_division(single_3_bulletRect[i].y,40)][(single_3_bulletRect[i].x-80)/40 - 1] != 0 && single_3_bulletRect[i].x % 40 < 15)) {
                    single_3_bullet[i].is_active = 0;
                    single_3_bullet[i].player_id = 0;
                } 
                if (single_3_bullet[i].is_active == 1) {
                    single_3_bulletRect[i].x -= 8;
                }
            }
        }
    }
}



//// DUAL

void createDualMap1(int dual_map[12][12]) {
    FILE* inputFile = fopen("map/dual_stage_1_map.txt", "r");
    if (inputFile != NULL) {
        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 12; x++) {
                fscanf(inputFile, "%d", &dual_map[y][x]);
            }
        }
        fclose(inputFile);
    } else {
        fprintf(stderr, "Failed to open map.txt\n");
    }
}


void renderDual(SDL_Renderer *renderer, int dual_map[12][12], int map) {
    if (map == 1) {
        createDualMap1(dual_map);
    }

    int mapX = (640 - (12 * 40)) / 2;
    int mapY = (480 - (12 * 40)) / 2;

    SDL_Rect borderRect = { mapX - 2, mapY - 2, 12 * 40 + 4, 12 * 40 + 4 };
    SDL_RenderFillRect(renderer, &borderRect);

    // Render the map
    for (int y = 0; y < 12; y++) {
        for (int x = 0; x < 12; x++) {
            SDL_Rect destRect = { mapX + x * 40, mapY + y * 40, 40, 40 };
            SDL_Texture* texture = NULL;

            switch (dual_map[y][x]) {
                case 0:
                    break;
                case 1:
                    texture = block;
                    break;
                case 2:
                    texture = iron;
                    break;
                case 3: 
                    texture = grass;
                    break;
                case 4: 
                    texture = water;
                    break;
                case 8:
                    texture = friendTank;
                    break;
                case 9:
                    texture = myTank;
                    break;
                default:
                    break;
            }

            if (texture == meUp || texture == meDown || texture == meRight || texture == meLeft) {
                if (map == 1) {
                    SDL_RenderCopy(renderer, texture, NULL, &dual_controlRect);
                } 
                // else if (map == 2) {
                //    SDL_RenderCopy(renderer, texture, NULL, &single_controlRect_2);
                // } else if (map == 3) {
                //     SDL_RenderCopy(renderer, texture, NULL, &single_controlRect_3);
                // }
            } else if (texture == friendUp || texture == friendDown || texture == friendRight || texture == friendLeft) {
                if (map == 1) {
                    SDL_RenderCopy(renderer, texture, NULL, &dual_friendRect);
                }
            } else {
                SDL_RenderCopy(renderer, texture, NULL, &destRect);
            }
        }
    }
}

void dualMoveDown(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize, int single_map[12][12]) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (vertical_controller % stepEachBlock == 0 && *coor_y + 1 < 12 && single_map[*coor_y + 1][*coor_x] == 0) {
        if (horizontal_controller % stepEachBlock != 0 && horizontal_controller < *coor_x * stepEachBlock) {
            if (single_map[*coor_y + 1][*coor_x - 1] == 0 && single_map[*coor_y + 1][*coor_x] == 0) {
                ++vertical_controller;
                rect->y += stepSize;
            }
        } else if (horizontal_controller % stepEachBlock != 0 && horizontal_controller > *coor_x * stepEachBlock) {
            if (single_map[*coor_y + 1][*coor_x] == 0 && single_map[*coor_y + 1][*coor_x + 1] == 0) {
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
        updatePlayerPositionDual(coor_y, coor_x, 1, 0);
    } else if (vertical_controller % stepEachBlock != 0) {
        ++vertical_controller;
        rect->y += stepSize;
    }

    *position_x = horizontal_controller;
    *position_y = vertical_controller;
}

void dualMoveUp(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize, int single_map[12][12]) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (vertical_controller % stepEachBlock == 0 && *coor_y - 1 >= 0 && single_map[*coor_y - 1][*coor_x] == 0) {
        if (horizontal_controller % stepEachBlock != 0 && horizontal_controller < *coor_x * stepEachBlock) {
            if (single_map[*coor_y - 1][*coor_x - 1] == 0 && single_map[*coor_y - 1][*coor_x] == 0) {
                --vertical_controller;
                rect->y -= stepSize;
            }
        } else if (horizontal_controller % stepEachBlock != 0 && horizontal_controller > *coor_x * stepEachBlock) {
            if (single_map[*coor_y - 1][*coor_x] == 0 && single_map[*coor_y - 1][*coor_x + 1] == 0) {
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
        updatePlayerPositionDual(coor_y, coor_x, -1, 0);
    } else if (vertical_controller % stepEachBlock != 0) {
        --vertical_controller;
        rect->y -= stepSize;
        
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void dualMoveRight(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize, int single_map[12][12]) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    // Check if moving right is possible and handle the cases
    if (horizontal_controller % stepEachBlock == 0 && *coor_x + 1 < 12 && single_map[*coor_y][*coor_x + 1] == 0) {
        if (vertical_controller % stepEachBlock != 0 && vertical_controller < *coor_y * stepEachBlock) {
            if (single_map[*coor_y][*coor_x + 1] == 0 && single_map[*coor_y - 1][*coor_x + 1] == 0) {
                ++horizontal_controller;
                rect->x += stepSize;
            }
        } else if (vertical_controller % stepEachBlock != 0 && vertical_controller > *coor_y * stepEachBlock) {
            if (single_map[*coor_y + 1][*coor_x + 1] == 0 && single_map[*coor_y][*coor_x + 1] == 0) {
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
        updatePlayerPositionDual(coor_y, coor_x, 0, 1);
    } else if (horizontal_controller % stepEachBlock != 0) {
        ++horizontal_controller;
        rect->x += stepSize;
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void dualMoveLeft(int *position_x, int *position_y, int *coor_x, int *coor_y, SDL_Rect *rect, int stepEachBlock, int stepSize, int single_map[12][12]) {
    int vertical_controller = *position_y;
    int horizontal_controller = *position_x;

    if (horizontal_controller % stepEachBlock == 0 && *coor_x - 1 >= 0 && single_map[*coor_y][*coor_x - 1] == 0) {
        if (vertical_controller % stepEachBlock != 0 && vertical_controller < *coor_y * stepEachBlock) {
            if (single_map[*coor_y][*coor_x - 1] == 0 && single_map[*coor_y - 1][*coor_x - 1] == 0) {
                --horizontal_controller;
                rect->x -= stepSize;
            }
        } else if (vertical_controller % stepEachBlock != 0 && vertical_controller > *coor_y * stepEachBlock) {
            if (single_map[*coor_y][*coor_x - 1] == 0 && single_map[*coor_y + 1][*coor_x - 1] == 0) {
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
        updatePlayerPositionDual(coor_y, coor_x, 0, -1);
    } else if (horizontal_controller % stepEachBlock != 0) {
        --horizontal_controller;
        rect->x -= stepSize;
    }

    *position_x = horizontal_controller; // Update position_x
    *position_y = vertical_controller;   // Update position_y
}

void dualShot(SDL_Texture *tank, SDL_Rect rect, SDL_Texture *meUp, SDL_Texture *meDown, SDL_Texture *meRight, SDL_Texture *meLeft, int player_shot) {
    for (int i =0; i < 30; i ++) {
        if (dual_bullet[i].is_active == 0) {
            dual_bullet[i].is_active = 1;

            dual_bullet[i].player_id = myId;
           
            dual_bulletRect[i].x = rect.x;
            dual_bulletRect[i].y = rect.y;

            if (tank == meUp) {
                dual_bullet[i].direction = UP;
                bulletTexture[i] = *(&bullet_up);
            } else if (tank == meDown) {
                dual_bullet[i].direction = DOWN;
                bulletTexture[i] = *(&bullet_down);

            } else if (tank == meRight) {
                dual_bullet[i].direction = RIGHT;
                bulletTexture[i] = *(&bullet_right);

            } else {
                dual_bullet[i].direction = LEFT;
                bulletTexture[i] = *(&bullet_left);

            }
            break;
        }
    }
}

void dualShotFriend(SDL_Texture *tank, SDL_Rect rect, SDL_Texture *meUp, SDL_Texture *meDown, SDL_Texture *meRight, SDL_Texture *meLeft, int player_shot) {
    for (int i =0; i < 30; i ++) {
        if (dual_bullet_friend[i].is_active == 0) {
            dual_bullet_friend[i].is_active = 1;

            dual_bullet_friend[i].player_id = myId;
           
            dual_bulletRect_friend[i].x = rect.x;
            dual_bulletRect_friend[i].y = rect.y;

            if (tank == meUp) {
                dual_bullet_friend[i].direction = UP;
                bulletTexture[i] = *(&bullet_up);
            } else if (tank == meDown) {
                dual_bullet_friend[i].direction = DOWN;
                bulletTexture[i] = *(&bullet_down);

            } else if (tank == meRight) {
                dual_bullet_friend[i].direction = RIGHT;
                bulletTexture[i] = *(&bullet_right);

            } else {
                dual_bullet_friend[i].direction = LEFT;
                bulletTexture[i] = *(&bullet_left);

            }
            break;
        }
    }
}

void renderBulletDual(SDL_Renderer *renderer) {
    for (int i = 0; i < 30; i ++) {
        if (dual_bullet[i].is_active == 1) {
            SDL_RenderCopy(renderer, bulletTexture[i], NULL, &dual_bulletRect[i]);
        }
        if (dual_bullet[i].is_active == 1) {
            if (dual_bullet[i].direction == UP) {

                if (dual_bullet[i].player_id == myId && dual_bulletRect[i].y >= dual_friendRect.y && dual_friendRect.y >= dual_bulletRect[i].y - 2
                    && dual_bulletRect[i].x > dual_friendRect.x - 20 && dual_bulletRect[i].x < dual_friendRect.x + 20 && myId != 0
                ) {
                    single_scores++;

                    friendTank = IMG_LoadTexture(renderer, "images/hitted.png");
                    SDL_RenderCopy(renderer, friendTank, NULL, &dual_friendRect);
                    
                    SDL_DestroyTexture(friendTank);

                    friendTank = IMG_LoadTexture(renderer, "images/friend_up.png");
                    dual_friendRect = (SDL_Rect) { 400, 440, 40, 40 };
                    SDL_RenderCopy(renderer, friendTank, NULL, &dual_friendRect);

                    dual_bullet[i].is_active = 0;
                    dual_bullet[i].player_id = 0;
                }

                if (dual_bulletRect[i].y < 1 || (dual_map_1[(dual_bulletRect[i].y)/40 - 1][round_integer_division(dual_bulletRect[i].x-80, 40)] != 0 && dual_bulletRect[i].y % 40 < 15)) {
                    dual_bullet[i].is_active = 0;
                    dual_bullet[i].player_id = 0;
                }

                if (dual_bullet[i].is_active == 1) {
                    dual_bulletRect[i].y -= 1;
                }
            } else if (dual_bullet[i].direction == DOWN) {
                if (dual_bullet[i].player_id == myId && dual_bulletRect[i].y <= dual_friendRect.y && dual_friendRect.y <= dual_bulletRect[i].y + 2
                    && dual_bulletRect[i].x > dual_friendRect.x - 20 && dual_bulletRect[i].x < dual_friendRect.x + 20 && myId != 0
                ) {

                    single_scores++;

                    friendTank = IMG_LoadTexture(renderer, "images/hitted.png");
                    SDL_RenderCopy(renderer, friendTank, NULL, &dual_friendRect);
                    
                    friendTank = IMG_LoadTexture(renderer, "images/friend_up.png");
                    dual_friendRect = (SDL_Rect) { 400, 440, 40, 40 };
                    SDL_RenderCopy(renderer, friendTank, NULL, &dual_friendRect);

                    dual_bullet[i].is_active = 0;
                    dual_bullet[i].player_id = 0;
                }
            
                if (dual_bulletRect[i].y > 440 || (dual_map_1[(dual_bulletRect[i].y)/40 + 2][round_integer_division(dual_bulletRect[i].x-80, 40)] != 0 && dual_bulletRect[i].y % 40 > 15)) {
                    dual_bullet[i].is_active = 0;
                    dual_bullet[i].player_id = 0;
                }
                
                if (dual_bullet[i].is_active == 1) {
                    dual_bulletRect[i].y += 1;
                }
            } else if (single_3_bullet[i].direction == RIGHT) {
                
                if (dual_bullet[i].player_id == myId && dual_bulletRect[i].x <= dual_friendRect.x  && dual_friendRect.x <= dual_bulletRect[i].x + 2
                    && dual_bulletRect[i].y > dual_friendRect.y - 20 && dual_bulletRect[i].y < dual_friendRect.y + 20 && myId != 0
                ) {

                    single_scores++;

                    friendTank = IMG_LoadTexture(renderer, "images/hitted.png");
                    SDL_RenderCopy(renderer, friendTank, NULL, &dual_friendRect);
                    
                    friendTank = IMG_LoadTexture(renderer, "images/friend_up.png");
                    dual_friendRect = (SDL_Rect) { 400, 440, 40, 40 };
                    SDL_RenderCopy(renderer, friendTank, NULL, &dual_friendRect);

                    dual_bullet[i].is_active = 0;
                    dual_bullet[i].player_id = 0;

                }
                if (dual_bulletRect[i].x > 520 || (dual_map_1[round_integer_division(dual_bulletRect[i].y,40)][(dual_bulletRect[i].x-80)/40 + 2] != 0 && dual_bulletRect[i].x % 40 > 15)) {
                    dual_bullet[i].is_active = 0;
                    dual_bullet[i].player_id = 0;
                } 

                if (dual_bullet[i].is_active == 1) {
                    dual_bulletRect[i].x += 1;
                }
            } else {
                if (dual_bullet[i].player_id == myId && dual_bulletRect[i].x >= dual_friendRect.x && dual_friendRect.x >= dual_bulletRect[i].x - 2
                    && dual_bulletRect[i].y > dual_friendRect.y - 20 && dual_bulletRect[i].y < dual_friendRect.y + 20 && myId != 0
                ) {
                    single_scores++;

                    friendTank = IMG_LoadTexture(renderer, "images/hitted.png");
                    SDL_RenderCopy(renderer, friendTank, NULL, &dual_friendRect);

                    SDL_DestroyTexture(friendTank);
                    
                    friendTank = IMG_LoadTexture(renderer, "images/friend_up.png");
                    dual_friendRect = (SDL_Rect) { 400, 440, 40, 40 };
                    SDL_RenderCopy(renderer, friendTank, NULL, &dual_friendRect);

                    dual_bullet[i].is_active = 0;
                    dual_bullet[i].player_id = 0;
                }
            
                if (dual_bulletRect[i].x < 2 || (dual_map_1[round_integer_division(dual_bulletRect[i].y,40)][(dual_bulletRect[i].x-80)/40 - 1] != 0 && dual_bulletRect[i].x % 40 < 15)) {
                    dual_bullet[i].is_active = 0;
                    dual_bullet[i].player_id = 0;
                } 
                if (dual_bullet[i].is_active == 1) {
                    dual_bulletRect[i].x -= 1;
                }
            }
        }
    }
}

void renderBulletDual_friend(SDL_Renderer *renderer) {
    for (int i = 0; i < 30; i ++) {
        if (dual_bullet_friend[i].is_active == 1) {
            SDL_RenderCopy(renderer, bulletTexture[i], NULL, &dual_bulletRect_friend[i]);
        }
        if (dual_bullet_friend[i].is_active == 1) {
            if (dual_bullet_friend[i].direction == UP) {
                
                printf("dual_bulletRect_friend[i].y: %d\n", dual_bulletRect_friend[i].y);
                printf("dual_controlRect.y: %d\n", dual_controlRect.y);
                printf("dual_bulletRect_friend[i].y - 2: %d\n", dual_bulletRect_friend[i].y - 2);

                if (dual_bullet_friend[i].player_id == myId && dual_bulletRect_friend[i].y >= dual_controlRect.y && dual_controlRect.y >= dual_bulletRect_friend[i].y - 2
                    && dual_bulletRect_friend[i].x > dual_controlRect.x - 20 && dual_bulletRect_friend[i].x < dual_controlRect.x + 20 && myId != 0
                ) {
                    single_scores++;

                    myTank = IMG_LoadTexture(renderer, "images/hitted.png");
                    SDL_RenderCopy(renderer, myTank, NULL, &dual_controlRect);
                    
                    SDL_DestroyTexture(myTank);

                    myTank = IMG_LoadTexture(renderer, "images/me_up.png");
                    dual_controlRect = (SDL_Rect) { 240, 440, 40, 40 };
                    SDL_RenderCopy(renderer, myTank, NULL, &dual_controlRect);

                    dual_bullet_friend[i].is_active = 0;
                    dual_bullet_friend[i].player_id = 0;
                }

                if (dual_bulletRect_friend[i].y < 1 || (dual_map_1[(dual_bulletRect_friend[i].y)/40 - 1][round_integer_division(dual_bulletRect_friend[i].x-80, 40)] != 0 && dual_bulletRect_friend[i].y % 40 < 15)) {
                    dual_bullet_friend[i].is_active = 0;
                    dual_bullet_friend[i].player_id = 0;
                }

                if (dual_bullet_friend[i].is_active == 1) {
                    dual_bulletRect_friend[i].y -= 1;
                }
            } else if (dual_bullet_friend[i].direction == DOWN) {
                printf("dual_bulletRect_friend[i].y: %d\n", dual_bulletRect_friend[i].y);
                printf("dual_controlRect.y: %d\n", dual_controlRect.y);
                printf("dual_bulletRect_friend[i].y - 2: %d\n", dual_bulletRect_friend[i].y + 2);
                if (dual_bullet_friend[i].player_id == myId && dual_bulletRect_friend[i].y <= dual_controlRect.y && dual_controlRect.y <= dual_bulletRect_friend[i].y + 2
                    && dual_bulletRect_friend[i].x > dual_controlRect.x - 20 && dual_bulletRect_friend[i].x < dual_controlRect.x + 20 && myId != 0
                ) {

                    single_scores++;

                    myTank = IMG_LoadTexture(renderer, "images/hitted.png");
                    SDL_RenderCopy(renderer, myTank, NULL, &dual_controlRect);
                    
                    myTank = IMG_LoadTexture(renderer, "images/me_up.png");
                    dual_controlRect = (SDL_Rect) { 240, 440, 40, 40 };
                    SDL_RenderCopy(renderer, myTank, NULL, &dual_controlRect);

                    dual_bullet_friend[i].is_active = 0;
                    dual_bullet_friend[i].player_id = 0;
                }
            
                if (dual_bulletRect_friend[i].y > 440 || (dual_map_1[(dual_bulletRect_friend[i].y)/40 + 2][round_integer_division(dual_bulletRect_friend[i].x-80, 40)] != 0 && dual_bulletRect_friend[i].y % 40 > 15)) {
                    printf("come here\n");
                    dual_bullet_friend[i].is_active = 0;
                    dual_bullet_friend[i].player_id = 0;
                }
                
                if (dual_bullet_friend[i].is_active == 1) {
                    dual_bulletRect_friend[i].y += 1;
                }
            } else if (single_3_bullet[i].direction == RIGHT) {
                
                if (dual_bullet_friend[i].player_id == myId && dual_bulletRect_friend[i].x <= dual_controlRect.x  && dual_controlRect.x <= dual_bulletRect_friend[i].x + 2
                    && dual_bulletRect_friend[i].y > dual_controlRect.y - 20 && dual_bulletRect_friend[i].y < dual_controlRect.y + 20 && myId != 0
                ) {

                    single_scores++;

                    myTank = IMG_LoadTexture(renderer, "images/hitted.png");
                    SDL_RenderCopy(renderer, myTank, NULL, &dual_controlRect);
                    
                    myTank = IMG_LoadTexture(renderer, "images/me_up.png");
                    dual_controlRect = (SDL_Rect) { 240, 440, 40, 40 };
                    SDL_RenderCopy(renderer, myTank, NULL, &dual_controlRect);

                    dual_bullet_friend[i].is_active = 0;
                    dual_bullet_friend[i].player_id = 0;

                }
                if (dual_bulletRect_friend[i].x > 520 || (dual_map_1[round_integer_division(dual_bulletRect_friend[i].y,40)][(dual_bulletRect_friend[i].x-80)/40 + 2] != 0 && dual_bulletRect_friend[i].x % 40 > 15)) {
                    dual_bullet_friend[i].is_active = 0;
                    dual_bullet_friend[i].player_id = 0;
                } 

                if (dual_bullet_friend[i].is_active == 1) {
                    dual_bulletRect_friend[i].x += 1;
                }
            } else {
                if (dual_bullet_friend[i].player_id == myId && dual_bulletRect_friend[i].x >= dual_controlRect.x && dual_controlRect.x >= dual_bulletRect_friend[i].x - 2
                    && dual_bulletRect_friend[i].y > dual_controlRect.y - 20 && dual_bulletRect_friend[i].y < dual_controlRect.y + 20 && myId != 0
                ) {
                    single_scores++;

                    myTank = IMG_LoadTexture(renderer, "images/hitted.png");
                    SDL_RenderCopy(renderer, myTank, NULL, &dual_controlRect);
                    
                    myTank = IMG_LoadTexture(renderer, "images/me_up.png");
                    dual_controlRect = (SDL_Rect) { 240, 440, 40, 40 };
                    SDL_RenderCopy(renderer, myTank, NULL, &dual_controlRect);

                    dual_bullet_friend[i].is_active = 0;
                    dual_bullet_friend[i].player_id = 0;
                }
            
                if (dual_bulletRect_friend[i].x < 2 || (dual_map_1[round_integer_division(dual_bulletRect_friend[i].y,40)][(dual_bulletRect_friend[i].x-80)/40 - 1] != 0 && dual_bulletRect_friend[i].x % 40 < 15)) {
                    dual_bullet_friend[i].is_active = 0;
                    dual_bullet_friend[i].player_id = 0;
                } 
                if (dual_bullet_friend[i].is_active == 1) {
                    dual_bulletRect_friend[i].x -= 1;
                }
            }
        }
    }
}


#endif