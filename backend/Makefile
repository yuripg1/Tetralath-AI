CC = gcc
CFLAGS = -O2 -flto -Wall -fPIC
LDLIBS = -lncurses -lpthread

# Standalone build (no debug)
SRCS = main.c game.c ai.c ui.c time.c
OBJS = $(SRCS:.c=.o)

# Standalone build with debug
SRCS_DEBUG = main.c game.c ai.c ui.c time.c debug.c
OBJS_DEBUG = $(SRCS_DEBUG:.c=.o)

# Shared library (no debug): game engine + AI + time; only game.h symbols exported
LIB_OBJS = game.o ai.o time.o

# Shared library with debug
LIB_OBJS_DEBUG = game.o ai.o time.o debug.o

# Version script: export only public API from game.h
LIB_SYMS = libtetralath.syms

# Default: standalone without debug
tetralath_ai: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

# Standalone with debug
tetralath_ai_debug: $(OBJS_DEBUG)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_DEBUG) $(LDLIBS)

# Library without debug; only game.h functions are exposed
libtetralath.so: $(LIB_OBJS)
	$(CC) $(LDFLAGS) -shared -o $@ $(LIB_OBJS) -lpthread -Wl,--version-script=$(LIB_SYMS)

# Library with debug
libtetralath_debug.so: $(LIB_OBJS_DEBUG)
	$(CC) $(LDFLAGS) -shared -o $@ $(LIB_OBJS_DEBUG) -lpthread -Wl,--version-script=$(LIB_SYMS)

all: tetralath_ai

clean:
	rm -f tetralath_ai tetralath_ai_debug libtetralath.so libtetralath_debug.so *.o

.PHONY: all clean lib lib_debug debug
lib: libtetralath.so
debug: tetralath_ai_debug
lib_debug: libtetralath_debug.so
