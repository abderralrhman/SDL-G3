CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

OBJ = main.o background.o

prog: $(OBJ)
	$(CC) $(OBJ) -o prog $(LIBS)

main.o: main.c background.h
	$(CC) $(CFLAGS) -c main.c

background.o: background.c background.h
	$(CC) $(CFLAGS) -c background.c

clean:
	rm -f *.o prog
