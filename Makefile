AR		= ar
AFLAGS	= rcs
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -Wpedantic -std=c99
SRCS	= $(addsuffix .c, ./simple)
OBJS	= $(SRCS:.c=.o)
TARGET	= libsimple.a

.PHONY : release debug

release : CFLAGS += -s -O3 -DRELEASE
release : all

debug : CFLAGS += -ggdb3 -O0 -DDEBUG
debug : all

all : CFLAGS += -DSIMPLE_BACKEND_OPENGL
# all : CFLAGS += -DSIMPLE_BACKEND_SOFTWARE
all : $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) $(AFLAGS) $@ $^

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean

clean :
	rm -f $(OBJS)
	rm -f $(TARGET)
