#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./vm.c"

void test_cases()
{

  /**
   * @brief Primeiras intruções funcionais
   */
  {
    inst_t instructions[] = {
      INST(PUSH, 97),
      INST(PUSH, 97),
      INST(PUSH, 3),
      INST(PLUS, 0),
      INST(PUSH, 97),
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.index == 3 && "O programa deveria deixar três valores na stack");

    dump_stack_memory(&vm);
  }

  {
    /**
     * @brief programa que gera a Sequência de Fibonacci, porém como cada célula da stack tem 8 bits,
     * não vamos muito longe nos cálculos sem sofrermos overflow e a célula fazer um  "wrap around".
     */
    inst_t instructions[] = {
      INST(PUSH, 0),
      INST(PUSH, 1),
      INST(DUP, 1),
      INST(DUP, 1),
      INST(PLUS, 0),
      INST(JUMP, 2),
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    dump_stack_memory(&vm);
  }
    
  {
    inst_t instructions[] = {
      INST(PUSH, 3),
      INST(PUSH, 4),
      INST(DUP, 0),
      INST(DUP, 2),
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.stack[2] == 4 && "O valor 2 deveria estar nesse endereço");
    assert(vm.stack[3] == 3 && "O valor 3 deveria estar nesse endereço");

    dump_stack_memory(&vm);
  }

  {
    inst_t instructions[] = {
      INST(PUSH, 1),
      INST(PUSH, 0),
      INST(DUP, 1),
      INST(JUMP_NOT_ZERO, 2),
      INST(PUSH, 5),
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.stack[2] == 1 && "O valor 1 deveria estar nesse endereço");
    assert(vm.stack[3] == 0 && "O valor 0 deveria estar nesse endereço");
    assert(vm.stack[4] == 5 && "O valor 5 deveria estar nesse endereço");
    assert(vm.stack[5] == 0 && "O valor 0 deveria estar nesse endereço");

    dump_stack_memory(&vm);
  }

  {
    inst_t instructions[] = {
      INST(PUSH, 10),
      INST(DUP, 0),
      INST(PUSH, 1),
      INST(SUB, 0),
      INST(JUMP_NOT_ZERO, 1)
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.stack[0] == 10 && "O valor 10 deveria estar nesse endereço");
    assert(vm.stack[9] == 1 && "O valor 1 deveria estar nesse endereço");

    dump_stack_memory(&vm);
  }

  {
    inst_t instructions[] = {
      INST(PUSH, 10),
      INST(PUSH, 9),
      INST(POP, 0),
      INST(PUSH, 8)
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.stack[0] == 10 && "O valor 10 deveria estar nesse endereço");
    assert(vm.stack[1] == 8 && "O valor 8 deveria estar nesse endereço");

    dump_stack_memory(&vm);
  }

  {
    inst_t instructions[] = {
      INST(PUSH, 10),
      INST(JUMP_ZERO, 5),
      INST(PUSH, 1),
      INST(SUB, 0),
      INST(JUMP, 1),
      INST(PUSH, 23),
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.stack[0] == 0 && "O valor 0 deveria estar nesse endereço");
    assert(vm.stack[1] == 23 && "O valor 23 deveria estar nesse endereço");

    dump_stack_memory(&vm);
  }

  {
    inst_t instructions[] = {
      INST(PUSH, 10),
      INST(PUSH, 5),
      INST(GREATER_THAN, 0),
      INST(PUSH, 5),
      INST(PUSH, 10),
      INST(GREATER_THAN, 0),
      INST(PUSH, 10),
      INST(PUSH, 5),
      INST(LOWER_THAN, 0),
      INST(PUSH, 5),
      INST(PUSH, 10),
      INST(LOWER_THAN, 0),
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.stack[0] == 1 && "O valor 1 deveria estar nesse endereço");
    assert(vm.stack[1] == 0 && "O valor 0 deveria estar nesse endereço");
    assert(vm.stack[2] == 0 && "O valor 0 deveria estar nesse endereço");
    assert(vm.stack[3] == 1 && "O valor 1 deveria estar nesse endereço");

    dump_stack_memory(&vm);
  }

  {
    inst_t instructions[] = {
      INST(PUSH, 10),
      INST(PUSH, 5),
      INST(HALT, 0),
      INST(PUSH, 23),
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.stack[0] == 10 && "O valor 10 deveria estar nesse endereço");
    assert(vm.stack[1] == 5 && "O valor 5 deveria estar nesse endereço");
    assert(vm.index == 2 && "O tamanho da stack deveria ser 2");
    assert(vm.stack[2] == 0 && "O valor 0 deveria estar nesse endereço");

    dump_stack_memory(&vm);
  }

  {
    inst_t instructions[] = {
      INST(PUSH, 97),
      INST(JUMP, 3),
      INST(POP, 0),
      INST(WRITE, 0),
      INST(PUSH, 1),
      INST(PLUS, 0),
      INST(DUP, 0),
      INST(PUSH, 105),
      INST(LOWER_THAN, 0),
      INST(JUMP_NOT_ZERO, 2),
    };

    vm_instance_t vm = {0};
    vm.program = (program_t) {
      .instructions = (inst_t *) &instructions,
      .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
    };

    execute_program(&vm);

    assert(vm.stack[0] == 105 && "O valor 105 deveria estar nesse endereço");
    assert(vm.stack[1] == 0 && "O valor 0 deveria estar nesse endereço");
    assert(vm.stack[2] == 105 && "O valor 105 deveria estar nesse endereço"); //  Esse 105 não é apagado pelo comando lower_than
    assert(vm.index == 2 && "O tamanho da stack deveria ser 2");

    dump_stack_memory(&vm);
  }

}

int main()
{
  test_cases();

  return EXIT_SUCCESS;
}
