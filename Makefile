main: main.c
	$(CC) main.c -o bin/a.out -Wall -Wextra -pedantic -Wswitch-enum -std=c11

run: main
	./bin/a.out
