all: hashmap

hashmap: main.c hashmap.c hashmap.h
	gcc -O2 -Wall -pedantic -Wextra main.c hashmap.c -o hashmap
	./hashmap

.PHONY: clean
clean:
	rm -f hashmap
