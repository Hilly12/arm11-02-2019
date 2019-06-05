CC      = gcc
CFLAGS  = -Wall -g -DEFAULT_SOURCE -std=c99 -pedantic
DEPS    = pipeline.h decode.h execute.h utils.h definitions.h
DEPSAS  = symbolTable.h ioManager.h
OBJ     = emulate.o pipeline.o decode.o execute.o utils.o
OBJAS   = assemble.o symbolTable.o ioManager.o
LIBS    = -lm

.SUFFIXES: .c .o

.PHONY: all clean

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.c $(DEPSAS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: emulate assemble

emulate: $(OBJ)
	$(CC) $(OBJ) -o emulate $(CFLAGS) $(LIBS)

assemble: $(OBJAS)
	$(CC) $(OBJAS) -o assemble $(CFLAGS) $(LIBS)

clean:
	rm -f $(wildcard *.o)
	rm -f emulate
	rm -f assemble
