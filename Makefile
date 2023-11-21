CFLAGS = -c -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf
CC = gcc
LIBS =  -lm

all: main

debug: CFLAGS += -g
debug: main

main: main.c
	${CC} main.c -o main `sdl2-config --cflags --libs` -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf

clean:
	rm -f *.o *~