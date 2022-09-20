#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define STACK_MAX_SIZE 255

#define INST(name, value) ((inst_t) { .type = INST_##name, .operand = value })


typedef uint8_t word_t;

/**
 * @brief Conjunto de intruções suportadas
 * 
 */
typedef enum instructions {
  /**
   * @note wrap around é o comportamento esperado para overflow
   */
  INST_PLUS,
  /**
   * @note wrap around é o comportamento esperado para undeflow
   */
  INST_SUB,
  INST_PUSH,
  INST_JUMP,
  INST_DUP,
  // O Comando pop apenas decrementa o ponteiro da stack, ele não zera a celula liberada
  INST_POP,
  INST_JUMP_NOT_ZERO,
  INST_JUMP_ZERO,
} instructions_t;

enum signals {
  OK,
  STACK_OVERFLOW,
  STACK_UNDERFLOW,
  INVALID_JUMP_POSITION,
  INVALID_OPERAND,
};

typedef struct inst {
  instructions_t type;
  uint8_t operand;
} inst_t;


static inline inst_t inst(instructions_t inst, uint8_t operand)
{
  return (inst_t) { .type = inst, .operand = operand };
}

typedef struct program {
  inst_t *instructions;
  size_t number_of_instructions;
} program_t;

typedef struct vm_instance {
  word_t stack[STACK_MAX_SIZE];
  uint16_t index;
  size_t ip;
  program_t program;
} vm_instance_t;

enum signals execute_inst(vm_instance_t *vm, inst_t *inst)
{
  switch(inst->type) {
  case INST_PUSH:
    // @TODO João, problema aqui, nunca vai detectar o overflow
    // @NOTE problema parcialmente resolvido com o valor máximo de 255 pra stack, validar se ainda há algum efeito colateral
    if (vm->index >= STACK_MAX_SIZE) return STACK_OVERFLOW;
    vm->stack[vm->index++] = inst->operand;
  break;
  case INST_PLUS:
    if (vm->index < 2) return STACK_UNDERFLOW;
    vm->index -= 2;
    vm->stack[vm->index] =  vm->stack[vm->index] + vm->stack[vm->index + 1];
    vm->index++;
  break;
  case INST_SUB:
    if (vm->index < 2) return STACK_UNDERFLOW;
    vm->index -= 2;
    vm->stack[vm->index] =  vm->stack[vm->index] - vm->stack[vm->index + 1];
    vm->index++;
  break;
  case INST_JUMP:
    if (inst->operand >= vm->program.number_of_instructions) return INVALID_JUMP_POSITION;
    vm->ip = inst->operand;
    return OK;
  break;
  case INST_JUMP_NOT_ZERO:
    if (inst->operand >= vm->program.number_of_instructions) return INVALID_JUMP_POSITION;
    if (vm->stack[vm->index - 1]) {
      vm->ip = inst->operand;
      return OK;
    }
  break;
  case INST_JUMP_ZERO:
    if (inst->operand >= vm->program.number_of_instructions) return INVALID_JUMP_POSITION;
    if (vm->stack[vm->index - 1] == 0) {
      vm->ip = inst->operand;
      return OK;
    }
  break;
  case INST_DUP:
    if (inst->operand >= vm->index) return INVALID_OPERAND; // @TODO João, talvez um nome melhor
    vm->stack[vm->index] = vm->stack[vm->index - inst->operand - 1];
    vm->index++;
  break;
  case INST_POP:
    if (vm->index < 1) return STACK_UNDERFLOW;
    // @NOTE João, por hora decidi não zerar a celula "liberada"
    vm->index--;
  break;
  default:
    assert(0 || "Unreacheable");
  }

  vm->ip++;

  return OK;
}

const char *signal_to_name(enum signals signal)
{
  switch(signal) {
  case OK: return "OK";
  case STACK_OVERFLOW: return "STACK_OVERFLOW";
  case STACK_UNDERFLOW: return "STACK_UNDERFLOW";
  case INVALID_JUMP_POSITION: return "INVALID_JUMP_POSITION";
  case INVALID_OPERAND: return "INVALID_OPERAND";
  }
  assert(0 || "Unreacheable");
  return "";
}

void execute_program(vm_instance_t *vm)
{
  size_t max_execution_ticks = 75;

  while (vm->ip < vm->program.number_of_instructions && max_execution_ticks)
  {
    enum signals signal = execute_inst(vm, &vm->program.instructions[vm->ip]);

    if (signal != OK) {
      printf("Execução interrompida erro %s\n", signal_to_name(signal));
      goto end;
    }
    max_execution_ticks--;
  }

end:
  printf("ticks %ld\n", 75 - max_execution_ticks);
}

void dump_stack_memory(vm_instance_t *vm)
{
  printf("Stack Dump:\n");
  if (vm->index)
  {
    for (uint16_t i = 0; i < vm->index; i++)
    {
      printf(" %d: %d\n", i, vm->stack[i]);
    }
  }
  else
  {
    printf("  [stack vazia]");
  }
}

int main()
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

  return 0;
}
