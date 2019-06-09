CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g -D_DEFAULT_SOURCE -std=c11 -pedantic
DEPS    = pipeline.h decode.h execute.h utils.h definitions.h
DEPSAS  = parser.h symbol_table.h assemble_utils.h
OBJ     = emulate.o pipeline.o decode.o execute.o utils.o
OBJAS   = assemble.o parser.o symbol_table.o assemble_utils.o
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
