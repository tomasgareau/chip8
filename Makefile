OBJS = stack.c chip8.c main.c
TEST_OBJS = stack.c chip8.c list.c tests.c

CC = gcc

CFLAGS = -w

LFLAGS = `sdl-config --libs`

OBJ_NAME = chip8
TEST_OBJ_NAME = tests

DEBUG = -g

all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(OBJ_NAME)

debug: $(OBJS)
	$(CC) $(DEBUG) $(OBJS) $(CFLAGS) $(LFLAGS) -o $(OBJ_NAME)

tests: $(TEST_OBJS)
	$(CC) $(DEBUG) $(TEST_OBJS) -o $(TEST_OBJ_NAME)
