CC = gcc
CFLAGS = -O2 -flto -Wall -fPIC
LDLIBS = -lncurses -lpthread

SRCS = main.c game.c ai.c ui.c time.c debug.c
OBJS = $(SRCS:.c=.o)

# Object files for the shared library (game engine + AI + time + debug; no main, no ui)
LIB_OBJS = game.o ai.o time.o debug.o

tetralath_ai: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

libtetralath.so: $(LIB_OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $(LIB_OBJS) -lpthread

all: tetralath_ai

clean:
	rm -f tetralath_ai libtetralath.so $(OBJS)

.PHONY: all clean lib
lib: libtetralath.so
