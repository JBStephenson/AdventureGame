CFLAGS = -g -Wall -pthread
LDFLAGS = -pthread -lmxml

main: Game.c
	gcc Game.c -o main $(LDFLAGS)
