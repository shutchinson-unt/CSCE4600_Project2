CFLAGS=-Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith    \
	   -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5            \
	   -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default \
	   -Wswitch-enum -Wconversion -Wunreachable-code                   \
	   -std=gnu99 -pedantic -g -msse2

CC=gcc

SOURCES_COMMON=simulator.c mymalloc.c util.c

# problem 1 solution
SOURCES_PROB1=problem1.c $(SOURCES_COMMON)
OBJECTS_PROB1=$(SOURCES_PROB1:.c=.o)
EXECUTABLE_PROB1=problem1

# problem 2 solution
SOURCES_PROB2=problem2.c $(SOURCES_COMMON)
OBJECTS_PROB2=$(SOURCES_PROB2:.c=.o)
EXECUTABLE_PROB2=problem2

# problem 3 solution
SOURCES_PROB3=problem3.c $(SOURCES_COMMON)
OBJECTS_PROB3=$(SOURCES_PROB3:.c=.o)
EXECUTABLE_PROB3=problem3


all: $(EXECUTABLE_PROB1) $(EXECUTABLE_PROB2) $(EXECUTABLE_PROB3)

$(EXECUTABLE_PROB1): $(OBJECTS_PROB1)
	$(CC) $(OBJECTS_PROB1) -o $@

$(EXECUTABLE_PROB2): $(OBJECTS_PROB2)
	$(CC) $(OBJECTS_PROB2) -o $@

$(EXECUTABLE_PROB3): $(OBJECTS_PROB3)
	$(CC) $(OBJECTS_PROB3) -o $@

clean:
	rm -f $(EXECUTABLE_PROB1) $(EXECUTABLE_PROB2) $(EXECUTABLE_PROB3) \
		  $(OBJECTS_PROB1) $(OBJECTS_PROB2) $(OBJECTS_PROB3)
