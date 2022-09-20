main: main.c
	$(CC) main.c -o bin/a.out -Wall -Wextra -pedantic -Wswitch-enum -fshort-enums -std=c11

run: main
	./bin/a.out ./samples/bin/push10-push5-plus.vmp
