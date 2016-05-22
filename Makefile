CC=gcc
CFLAGS=-g `pkg-config --cflags x11` -DDEBUG
LFLAGS=`pkg-config --libs x11`

SRC=veta.c debug.c x11.c
OBJ=veta.o debug.o x11.o

all: veta

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

veta: $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS) $(OBJS)

clean:
	rm -f $(OBJ)
