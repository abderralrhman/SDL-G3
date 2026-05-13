CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
OBJ = main.o fonction.o
TARGET = sdl_menu

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

main.o: main.c header.h
	$(CC) $(CFLAGS) -c main.c

fonction.o: fonction.c header.h
	$(CC) $(CFLAGS) -c fonction.c

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
