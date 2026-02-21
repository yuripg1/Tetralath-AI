CC = gcc
CFLAGS = -Wall -Werror -O2 -flto

LDLIBS_STANDALONE = -lncurses -lpthread
LDLIBS_LIBRARY    = -lpthread

LIB_SYMS = libtetralath.syms

SRCS_STANDALONE       = main.c game.c ai.c ui.c time.c
SRCS_STANDALONE_DEBUG = main.c game.c ai.c ui.c time.c debug.c
OBJS_STANDALONE       = $(SRCS_STANDALONE:.c=.o)
OBJS_STANDALONE_DEBUG = $(SRCS_STANDALONE_DEBUG:.c=.o)

SRCS_LIBRARY       = game.c ai.c time.c
SRCS_LIBRARY_DEBUG = game.c ai.c time.c debug.c
OBJS_LIBRARY       = $(SRCS_LIBRARY:.c=.o)
OBJS_LIBRARY_DEBUG = $(SRCS_LIBRARY_DEBUG:.c=.o)

tetralath: $(OBJS_STANDALONE)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_STANDALONE) $(LDLIBS_STANDALONE)

tetralath_debug: $(OBJS_STANDALONE_DEBUG)
	$(CC) $(LDFLAGS) -o $@ $(OBJS_STANDALONE_DEBUG) $(LDLIBS_STANDALONE)

libtetralath.so: $(OBJS_LIBRARY)
	$(CC) $(LDFLAGS) -shared -o $@ $(OBJS_LIBRARY) $(LDLIBS_LIBRARY) -Wl,--version-script=$(LIB_SYMS)

libtetralath_debug.so: $(OBJS_LIBRARY_DEBUG)
	$(CC) $(LDFLAGS) -shared -o $@ $(OBJS_LIBRARY_DEBUG) $(LDLIBS_LIBRARY) -Wl,--version-script=$(LIB_SYMS)

all: tetralath

clean:
	rm -f tetralath tetralath_debug libtetralath.so libtetralath_debug.so *.o

.PHONY: all clean lib lib_debug debug
lib: libtetralath.so
debug: tetralath_debug
lib_debug: libtetralath_debug.so
