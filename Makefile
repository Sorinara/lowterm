.SUFFIXES : .c .o

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
CC = gcc
CFLAGS = -Wall -g $(INCLUDE)
#CFLAGS = -O -WAll

LIBS = `pkg-config --cflags --libs gtk+-2.0 gthread-2.0` -lvte -lconfuse -lrt  -lX11

APP_NAME = lowterm 
OBJS = main.c key_filter.c callback.c config.c terminal.c
SRCS = main.c

APP_NAME : $(OBJS)
	$(CC) -g -o $(APP_NAME) $(OBJS) $(LIBS)

main.o : $(SRCS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(APP_NAME)

install:
	cp $(APP_NAME) ~/bin/lowterm -f
