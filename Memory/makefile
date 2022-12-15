# Globales
CC = gcc
OUTFILE = memory.out

### Reglas ###

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)


LINK = -lpthread -lcommons -lm
CFLAGS = -c

build: makefolder $(OBJ)
	$(CC) -o $(OUTFILE) $(OBJ) $(LINK)

valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all --log-file=valgrind.log ./$(OUTFILE)

helgrind: build
	valgrind --tool=helgrind --log-file=helgrind.log ./$(OUTFILE)
debug: CFLAGS := $(CFLAGS) -g

./obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LINK)

makefolder:
	if [ ! -d ./obj ]; then mkdir obj; fi


# Clean

clean:
	rm -rf obj
	rm -rf *.out
	rm -rf *.log
	