.SUFFIXES : .c .o

CC = gcc
CFLAGS = -O -Wall -g `pkg-config --cflags gtk+-3.0 vte-2.91`

LIBS = `pkg-config --libs gtk+-3.0 vte-2.91` -lconfuse -lX11

APP_NAME = lowterm 
OBJS = main.c key_filter.c callback.c config.c terminal.c data.c
SRCS = main.c

APP_NAME : $(OBJS)
	$(CC) $(CFLAGS) -o $(APP_NAME) $(OBJS) $(LIBS)

main.o : $(SRCS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o $(APP_NAME)

install:
	cp $(APP_NAME) ~/bin/lowterm -f
