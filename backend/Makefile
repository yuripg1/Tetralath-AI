CC = gcc
CFLAGS = -O2 -flto -Wall
LDLIBS = -lncurses -lpthread

SRCS = main.c game.c ai.c ui.c time.c debug.c
OBJS = $(SRCS:.c=.o)

tetralath_ai: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

all: tetralath_ai

clean:
	rm -f tetralath_ai $(OBJS)

.PHONY: all clean
