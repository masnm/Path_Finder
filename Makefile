CC=gcc
flags=-g
sdl=`pkg-config sdl2 --libs`

all:
	$(CC) -o path_finder.out path_finder.c $(flags) $(sdl)

run:
	$(CC) -o path_finder.out path_finder.c $(flags) $(sdl)
	./path_finder.out

clean:
	rm ./path_finder.out
