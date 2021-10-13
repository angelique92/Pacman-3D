CC=g++

CFLAGS   = -Wunused -Wall -fPIC -I. -c -g

CFLAGS  += -DGTK_DISABLE_DEPRECATED=1 -DGDK_DISABLE_DEPRECATED
CFLAGS  += -DGDK_PIXBUF_DISABLE_DEPRECATED -DG_DISABLE_DEPRECATED
CFLAGS  += -DGTK_MULTIHEAD_SAFE=1 -DGTK_MULTIDEVICE_SAFE=1




FILES = main.o

.c.o:
	$(CC) -c $< $(CFLAGS)

all:$(FILES)
	$(CC) -o pacman $(FILES) -lGLU -lglut -lGL

clean:
	rm -f *.o pacman *core*

