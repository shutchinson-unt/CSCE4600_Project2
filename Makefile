CFLAGS=-Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code -std=gnu99 -pedantic -g -msse2
CC=gcc
SOURCES=simulator.c util.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=simulator

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
