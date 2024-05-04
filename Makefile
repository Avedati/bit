OBJECTS=build/main.o build/globals.o build/b_list.o build/b_string.o build/lexer.o build/parser.o
CC=gcc
CFLAGS=-c

all: bin/bit

debug:
	$(CC) $(CFLAGS) src/*.c -g -o bin/bit

bin/bit: $(OBJECTS)
	clang $(OBJECTS) -o bin/bit

build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) bin/bit tests/out/*

test:
	$(MAKE) all
	./bin/bit tests/in/custom_1.s > tests/out/custom_1.out
	diff tests/out/custom_1.out tests/ref/custom_1.ref
	$(MAKE) clean
