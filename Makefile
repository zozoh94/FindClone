CFLAGS = -W -Wall -std=gnu99 -O4
LDFLAGS =
DEP = pred.o parser.o util.o
CC = gcc

ifeq (${DEBUG}, 1)
	CFLAGS += -g -O0
endif

all: $(DEP)
	$(CC) $(addprefix bin/, $(DEP)) src/find.c -o bin/find.out $(CFLAGS) $(LDFLAGS)

%.o: src/%.c
	$(CC) -c $< -o bin/$@ $(CFLAGS)

#clean
clean:
	rm -rf bin/*.bak; rm -rf bin/*.o

#mrpropre
mrproper: clean
	rm -f bin/find
