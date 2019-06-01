CC      = gcc
CFLAGS  = -Wall -g -DEFAULT_SOURCE -std=c99 -pedantic
DEPS    = pipeline.h execute.h utils.h definitions.h
OBJ     = emulate.o pipeline.o execute.o utils.o
LIBS    = -lm

.SUFFIXES: .c .o

.PHONY: all clean

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: emulate

emulate: $(OBJ)
	$(CC) $(OBJ) -o emulate $(CFLAGS) $(LIBS)

clean:
	rm -f $(wildcard *.o)
	rm -f emulate
