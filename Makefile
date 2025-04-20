AR		= ar
AFLAGS	= rcs
CC		= cc
CFLAGS	= -Wall -Wextra -Werror -Wpedantic -std=c99
SRCS	= $(addsuffix .c, ./simple)
OBJS	= $(SRCS:.c=.o)
TARGET	= libsimple.a

$(TARGET) : $(OBJS)
	$(AR) $(AFLAGS) $@ $^

$(OBJS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : all

all : $(TARGET)

.PHONY : release

release : CFLAGS += -s -O3 -DRELEASE
release : all

.PHONY : debug

debug : CFLAGS += -ggdb3 -O0 -DDEBUG
debug : all

.PHONY : clean

clean :
	rm -f $(OBJS)
	rm -f $(TARGET)

.PHONY : install

install :
	sudo cp $(TARGET) /usr/local/lib
	sudo cp ./simple.h /usr/local/include
