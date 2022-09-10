#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define STACK_MAX_SIZE 1000

#define INST(name, value) ((inst_t) { .type = INST_##name, .operand = value })


typedef uint8_t word_t;

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


typedef struct vm_instance {
  word_t stack[STACK_MAX_SIZE];
  uint16_t index;
} vm_instance_t;

enum signals execute_inst(vm_instance_t *vm, inst_t *inst)
{
  switch(inst->type) {
  case INST_PUSH:
    if (vm->index >= STACK_MAX_SIZE) return STACK_OVERFLOW;
    // @TODO não seguro, terminar de validar stack antes de fazer o push
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

  return OK;
}

void dump_stack_memory(vm_instance_t *vm)
{
    printf("Stack:\n");
    if (vm->index)
    {
      for (uint16_t i = 0; i < vm->index; i++)
      {
        printf("  %d\n", vm->stack[i]);
      }
    }
    else
    {
      printf("  [stack vazia]");
    }
}

int main()
{
  vm_instance_t vm = {0};
  inst_t inst_a = INST(PUSH, 97);
  inst_t inst_b = INST(PUSH, 3);
  inst_t inst_plus = INST(PLUS, 0);
  
  execute_inst(&vm, &inst_a);
  execute_inst(&vm, &inst_a);
  execute_inst(&vm, &inst_b);
  execute_inst(&vm, &inst_plus);
  execute_inst(&vm, &inst_a);

  dump_stack_memory(&vm);
  printf("stack: %s ", (const char *) &vm.stack);
  return 0;
}
