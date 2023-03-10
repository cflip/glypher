CFLAGS=-std=c99 -g -Wall
LFLAGS=-lSDL2 -lz

OUT=glypher
OBJS=main.o     \
     editor.o   \
     error.o    \
     file.o     \
     font.o     \
     input.o    \
     line.o     \
     syntax.o   \
     textbuf.o  \
     window.o

DESTDIR=/usr/local

.PHONY: all clean install

all: $(OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJS)
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm -f $(OBJS) $(OUT)

install:
	mkdir -p $(DESTDIR)/bin/
	cp $(OUT) $(DESTDIR)/bin/

uninstall:
	rm -f $(DESTDIR)/bin/$(OUT)
