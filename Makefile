OBJS = stack.c chip8.c main.c

CC = gcc

CFLAGS = -w

LFLAGS = `sdl-config --libs`

OBJ_NAME = test

DEBUG = -g

all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(OBJ_NAME)

debug: $(OBJS)
	$(CC) $(DEBUG) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(OBJ_NAME)
