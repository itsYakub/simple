# -----------------------------------------------------------------------------

AR		= ar
AFLAGS	= rcs
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -Wpedantic -std=c99
SRCS	= ./simple.h
OBJS	= ./simple.o
TARGET	= libsimple.a

# -----------------------------------------------------------------------------

.PHONY : all

# This flag is responsible for including an implementation part of the header
# file during the preprocessing stage.
all : CFLAGS += -DSIMPLE_IMPLEMENTATION

# This flag is responsible for attaching proper parts of the internal API during
# the preprocessing stage. It's almost fully responsible for back - end code.
# Possible back - end implementations are as follows:
# - SIMPLE_BACKEND_OPENGL - OpenGL back - end;
# - SIMPLE_BACKEND_SOFTWARE - Software - Renderer back - end;
all : CFLAGS += -DSIMPLE_BACKEND_OPENGL 
all : $(TARGET)

# -----------------------------------------------------------------------------

$(TARGET) : $(OBJS)
	$(AR) $(AFLAGS) $@ $^

$(OBJS) : %.o : %.h
	$(CC) $(CFLAGS) -c $< -o $@

# -----------------------------------------------------------------------------

# Release configuration enables 'O3' optimization level for the library object.
# It also scrapes all the debug symbols, thus making the final output smaller.
# Lastly, it attaches the 'RELEASE' macro definition, which can be useful in
# the implementation.

.PHONY : release

release : CFLAGS += -s -O3 -DRELEASE
release : all

# -----------------------------------------------------------------------------

# Debug configuration enables 'O0' optimization level for the library object.
# It also leaves all the possible gdb debugger symbols, icluding all the macro
# definitions. Lastly, it attaches the 'DEBUG' macro definition, 
# which can be useful in the implementation.

.PHONY : debug

debug : CFLAGS += -ggdb3 -O0 -DDEBUG
debug : all

# -----------------------------------------------------------------------------

.PHONY : clean

clean :
	rm -f $(OBJS)
	rm -f $(TARGET)

# -----------------------------------------------------------------------------

# By default, 'install' rule will copy 'libsimple.a' file to '/usr/local/lib'
# The 'simple.h' header will be copied to the '/usr/local/include'

.PHONY : install

install :
	sudo cp $(TARGET) /usr/local/lib
	sudo cp ./simple.h /usr/local/include

# -----------------------------------------------------------------------------
