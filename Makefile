CFLAGS = -Wall -Wextra -pedantic -Wswitch-enum -fshort-enums -std=c11
# https://stackoverflow.com/questions/12824134/undefined-reference-to-pow-in-c-despite-including-math-h
LFLAGS = -lm

tests: src/test-cases.c
	$(CC) $(CFLAGS) src/test-cases.c -o bin/t.out $(LFLAGS)

main: src/main.c
	$(CC) $(CFLAGS) src/main.c -o bin/a.out $(LFLAGS)

tests-assembler:
	$(CC) $(CFLAGS) src/assembler/test-cases.c -o bin/tsvm-assembler-tests.out $(LFLAGS)

assembler: src/assembler/assembler.c
	$(CC) $(CFLAGS) src/assembler/main.c -o bin/tsvm-assembler.out $(LFLAGS)

run: tests main
	./bin/t.out > ./bin/test.log
	./bin/a.out ./samples/bin/push10-push5-plus.vmp

run-assembler: tests-assembler assembler
	./bin/tsvm-assembler-tests.out > ./bin/assembler-tests.log
	./bin/tsvm-assembler.out ./samples/asm/push10-push5-plus.tsasm	
