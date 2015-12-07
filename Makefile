all: hashmap test

hashmap: main.c hashmap.c hashmap.h
	gcc -g -O2 -Wall -pedantic -Wextra main.c hashmap.c -o hashmap

test: hashmap
	./hashmap

.PHONY: clean
clean:
	rm -f hashmap
	rm -rf hashmap.dSYM
