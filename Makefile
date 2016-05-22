CC=gcc
CFLAGS=-g `pkg-config --cflags x11` -DDEBUG
LFLAGS=`pkg-config --libs x11` -lm

SRC=veta.c debug.c x11.c conf.c cell.c color.c
OBJ=veta.o debug.o x11.o conf.o cell.o color.o

all: veta

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

veta: $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS) $(OBJS)

clean:
	rm -f $(OBJ)
