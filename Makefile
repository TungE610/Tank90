CFLAGS = -c -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf
CC = gcc
LIBS = -lm

all: client server

debug: CFLAGS += -g
debug: client server

client: client_udp.c constants.h objects.h messages.h authenticate.h variables.h fonts.h textures.h colors.h surfaces.h maps.h utils.h
	${CC} client_udp.c -o client `sdl2-config --cflags --libs` -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lpthread -lm -lssl -lcrypto

server: server_udp.c constants.h objects.h messages.h authenticate.h variables.h fonts.h textures.h colors.h surfaces.h
	${CC} server_udp.c -o server `sdl2-config --cflags --libs` -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lpthread -lm -lssl -lcrypto

clean:
	rm -f *.o *~
