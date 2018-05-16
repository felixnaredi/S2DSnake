S2D = \
	-lsimple2d -I/usr/include/SDL2 -I/usr/include -D_REENTRANT \
	-L/usr/lib -pthread -lSDL2 -lGL -lm -lSDL2_image -lSDL2_mixer -lSDL2_ttf

main: main.c
	gcc $^ -o $@.o $(S2D)

debug: main.c
	gcc $^ -o $@.o -g $(S2D)
