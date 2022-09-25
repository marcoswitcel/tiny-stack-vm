CFLAGS = -Wall -Wextra -pedantic -Wswitch-enum -fshort-enums -std=c11

tests: src/test-cases.c
	$(CC) $(CFLAGS) src/test-cases.c -o bin/t.out 

main: src/main.c
	$(CC) $(CFLAGS) src/main.c -o bin/a.out 

assembler: src/tsvm-assembler.c
	$(CC) $(CFLAGS) src/tsvm-assembler.c -o bin/tsvm-assembler.out 

run: tests main
	./bin/t.out > ./bin/test.log
	./bin/a.out ./samples/bin/push10-push5-plus.vmp
