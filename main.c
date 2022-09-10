#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define STACK_MAX_SIZE 1000

#define INST(name, value) ((inst_t) { .type = INST_##name, .operand = value })


typedef uint8_t word_t;

/**
 * @brief Conjunto de intruções suportadas
 * 
 */
typedef enum instructions {
  INST_PLUS,
  INST_PUSH,
} instructions_t;

enum signals {
  OK,
  STACK_OVERFLOW,
  STACK_UNDERFLOW,
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
  size_t sp;
  program_t program;
} vm_instance_t;

enum signals execute_inst(vm_instance_t *vm, inst_t *inst)
{
  switch(inst->type) {
  case INST_PUSH:
    if (vm->index >= STACK_MAX_SIZE) return STACK_OVERFLOW;
    vm->stack[vm->index++] = inst->operand;
  break;
  case INST_PLUS:
     if (vm->index < 2) return STACK_UNDERFLOW;
     vm->index -= 2;
     vm->stack[vm->index] =  vm->stack[vm->index] + vm->stack[vm->index + 1];
     vm->index++;
  break;
  default:
    assert(0 || "Unreacheable");
  }

  vm->sp++;

  return OK;
}

void execute_program(vm_instance_t *vm)
{
  while (vm->sp < vm->program.number_of_instructions)
  {
    execute_inst(vm, &vm->program.instructions[vm->sp]);
  }
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
  

  return 0;
}
