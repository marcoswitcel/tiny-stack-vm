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
  INST_JUMP,
} instructions_t;

enum signals {
  OK,
  STACK_OVERFLOW,
  STACK_UNDERFLOW,
  INVALID_JUMP_POSITION
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
  case INST_JUMP:
    if (inst->operand >= vm->program.number_of_instructions) return INVALID_JUMP_POSITION;
    vm->sp = inst->operand;
    return OK;
  break;
  default:
    assert(0 || "Unreacheable");
  }

  vm->sp++;

  return OK;
}

const char *signal_to_name(enum signals signal)
{
  switch(signal) {
  case OK: return "OK";
  case STACK_OVERFLOW: return "STACK_OVERFLOW";
  case STACK_UNDERFLOW: return "STACK_UNDERFLOW";
  case INVALID_JUMP_POSITION: return "INVALID_JUMP_POSITION";
  }
  assert(0 || "Unreacheable");
  return "";
}

void execute_program(vm_instance_t *vm)
{
  size_t max_execution_ticks = 2000;

  while (vm->sp < vm->program.number_of_instructions && max_execution_ticks)
  {
    enum signals signal = execute_inst(vm, &vm->program.instructions[vm->sp]);

    if (signal != OK) {
      printf("Execução interrompida erro %s\n", signal_to_name(signal));
      goto end;
    }
    max_execution_ticks--;
  }

end:
  printf("ticks %ld\n", 2000 - max_execution_ticks);
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
  inst_t instructions[] = {
    INST(PUSH, 1),
    INST(PUSH, 0),
    INST(PUSH, 0),
    INST(PLUS, 0),
    INST(JUMP, 0),
  };

  vm_instance_t vm = {0};
  vm.program = (program_t) {
    .instructions = (inst_t *) &instructions,
    .number_of_instructions = sizeof(instructions) / sizeof(instructions[0]),
  };

  execute_program(&vm);

  dump_stack_memory(&vm);
}
  

  return 0;
}
