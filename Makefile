CC=gcc
CFLAGS=-g `pkg-config --cflags x11`
LFLAGS=`pkg-config --libs x11`

SRC=veta.c
OBJ=veta.o

all: veta

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

veta: $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS) $(OBJS)

clean:
	rm -f $(OBJ) veta
