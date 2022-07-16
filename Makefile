CC=gcc
flags=-g
sdl=`pkg-config sdl2 --libs`

all:
	$(CC) -o main.out main.c $(flags) $(sdl)

run:
	$(CC) -o main.out main.c $(flags) $(sdl)
	./main.out

#all:
#	$(CC) -o path_finder.out path_finder.c $(flags) $(sdl)
#
#run:
#	$(CC) -o path_finder.out generator.c $(flags) $(sdl)
#	./path_finder.out
#
#clean:
#	rm ./path_finder.out
