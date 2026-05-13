prog: main.o minimap.o
	gcc main.o minimap.o -o prog -lSDL2 -lSDL2_image
main.o: main.c minimap.h
	gcc -c main.c
minimap.o: minimap.c minimap.h
	gcc -c minimap.c
clean:
	rm -f *.o prog
