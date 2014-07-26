.SUFFIXES : .c .o

CC = gcc
CFLAGS = -O -Wall -g

LIBS = `pkg-config --cflags --libs gtk+-2.0` -lvte -lconfuse -lX11

APP_NAME = lowterm 
OBJS = main.c key_filter.c callback.c config.c terminal.c data.c
SRCS = main.c

APP_NAME : $(OBJS)
	$(CC) -g -o $(APP_NAME) $(OBJS) $(LIBS)

main.o : $(SRCS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(APP_NAME)

install:
	cp $(APP_NAME) ~/bin/lowterm -f
