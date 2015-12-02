all:
	gcc -O2 -Wall -pedantic -Wextra main.c hashmap.c -o hashmap
	./hashmap

.PHONY: clean
clean:
	rm -f hashmap
