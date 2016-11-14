CC=gcc
VER=
CFLAGS=-g `pkg-config --cflags x11 xtst` 
LFLAGS=`pkg-config --libs x11 xtst` -lm
#CFLAGS=-g `pkg-config --cflags x11 xtst` -DDEBUG -pg
#LFLAGS=`pkg-config --libs x11 xtst` -lm -pg

SRC=veta.c debug.c x11.c conf.c cell.c color.c ui.c	ui_logic.c
OBJ=veta.o debug.o x11.o conf.o cell.o color.o ui.o ui_logic.o


all: veta

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

ver.h:
	./ver.sh > ver.h

veta: ver.h $(OBJ) 
	$(CC) -o $@ $^ $(LFLAGS) $(OBJS)

clean:
	rm -f $(OBJ) ver.h
