CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g -D_DEFAULT_SOURCE -std=c11 -pedantic
DEPS    = pipeline.h decode.h execute.h emulate_utils.h definitions.h
DEPSAS  = symbol_table.h parser.h assemble_utils.h definitions.h
OBJ     = emulate.o pipeline.o decode.o execute.o emulate_utils.o
OBJAS   = assemble.o symbol_table.o parser.o assemble_utils.o
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
