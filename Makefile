CC=gcc
CFLAGS=-Wall -Wextra $(shell sdl2-config --cflags)
LIBS=$(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
OBJ=main.o background.o menu.o player.o enemy.o minimap.o settings.o puzzle.o

prog: $(OBJ)
	$(CC) $(OBJ) -o prog $(LIBS)

main.o: main.c background.h menu.h player.h enemy.h minimap.h settings.h puzzle.h
	$(CC) $(CFLAGS) -c main.c

background.o: background.c background.h
	$(CC) $(CFLAGS) -c background.c

menu.o: menu.c menu.h
	$(CC) $(CFLAGS) -c menu.c

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c

enemy.o: enemy.c enemy.h background.h
	$(CC) $(CFLAGS) -c enemy.c

minimap.o: minimap.c minimap.h background.h
	$(CC) $(CFLAGS) -c minimap.c

settings.o: settings.c settings.h menu.h
	$(CC) $(CFLAGS) -c settings.c

puzzle.o: puzzle.c puzzle.h
	$(CC) $(CFLAGS) -c puzzle.c

clean:
	rm -f *.o prog
