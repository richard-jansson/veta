CC=gcc
VER=
CFLAGS=-g `pkg-config --cflags x11 xtst jansson` --std=gnu11 -DDEBUG 
LFLAGS=`pkg-config --libs x11 xtst jansson` -lm -g -DDEBUG 
#CFLAGS_DEBUG=-g `pkg-config --cflags x11 xtst` -DDEBUG 
#LFLAGS=`pkg-config --libs x11 xtst` -lm -pg

SRC=veta.c debug.c x11.c conf.c cell.c color.c ui.c ui_logic.c x11_getmodifiers.c jsonconf.c
OBJ=veta.o debug.o x11.o conf.o cell.o color.o ui.o ui_logic.o x11_getmodifiers.o jsonconf.o


all: veta

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

ver.h:
	./ver.sh > ver.h

veta: ver.h $(OBJ) 
	$(CC) -o $@ $^ $(LFLAGS) $(OBJS)

jansson: jansson/src/.libs/libjansson.so
	cd jansson
	make 
	cd ..

clean:
	rm -f $(OBJ) ver.h
