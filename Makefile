CFLAGS = -Wall -Wextra -pedantic -Wswitch-enum -fshort-enums -std=c11
# https://stackoverflow.com/questions/12824134/undefined-reference-to-pow-in-c-despite-including-math-h
LFLAGS = -lm

setup-build-dirs:
	@mkdir bin bin/tests bin/logs 

tests-vm: src/test-cases.c
	$(CC) $(CFLAGS) src/test-cases.c -o bin/tests/test-tsvm $(LFLAGS)

vm: src/main.c
	$(CC) $(CFLAGS) src/main.c -o bin/tsvm $(LFLAGS)

tests-assembler: src/assembler/test-cases.c
	$(CC) $(CFLAGS) src/assembler/test-cases.c -o bin/tests/test-tsassembler $(LFLAGS)

assembler: src/assembler/assembler.c
	$(CC) $(CFLAGS) src/assembler/main.c -o bin/tsassembler $(LFLAGS)

run-vm: tests-vm vm
	./bin/tests/test-tsvm > ./bin/logs/vm-tests.log
	./bin/tsvm ./samples/bin/push10-push5-plus.vmp

run-assembler: tests-assembler assembler
	./bin/tests/test-tsassembler > ./bin/logs/assembler-tests.log
	./bin/tsassembler ./samples/asm/push10-push5-plus.tsasm	
