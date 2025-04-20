CC		= cc
CFLAGS	= -Wall -Wextra -Werror -Wpedantic -std=c99 -ggdb3
LFLAGS	= -lsimple -lGL -lglad -lm

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
	LFLAGS += -lX11 -lGLX
else
	LFLAGS += -lgdi32
endif

all: 00-example-x11-opengl 01-example-x11-software

00-example-x11-opengl : 00-example-x11-opengl.c
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

01-example-x11-software : 01-example-x11-software.c
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)
