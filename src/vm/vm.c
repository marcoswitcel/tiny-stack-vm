#ifndef VM_H
#define VM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "./vm-definitions.h"

static inline inst_t inst(instructions_t inst, uint8_t operand)
{
  return (inst_t) { .type = inst, .operand = operand };
}

/**
 * @brief Executa uma instrução na máquina virtual
 * 
 * @param vm 
 * @param inst 
 * @return enum signals 
 */
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
  case INST_GREATER_THAN:
    if (vm->index < 2) return STACK_UNDERFLOW;
    vm->index -= 2;
    vm->stack[vm->index] =  vm->stack[vm->index] > vm->stack[vm->index + 1];
    vm->index++;
  break;
  case INST_LOWER_THAN:
    if (vm->index < 2) return STACK_UNDERFLOW;
    vm->index -= 2;
    vm->stack[vm->index] =  vm->stack[vm->index] < vm->stack[vm->index + 1];
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
  case INST_HALT:
    vm->halted = true;
  break;
  case INST_WRITE:
    // @todo João, revisar e talvez trocar o sinal retornado abaixo
    if (vm->index < 1) return STACK_UNDERFLOW;
    printf("%c", vm->stack[vm->index - 1]);
  break;
  case INST_SWAP:
    if (vm->index == 0 || inst->operand >= vm->index) return INVALID_OPERAND;
    uint8_t topIndex = vm->index - 1;
    uint8_t prevIndex = vm->index - (1 + inst->operand);
    word_t currValue = vm->stack[topIndex];

    vm->stack[topIndex] = vm->stack[prevIndex];
    vm->stack[prevIndex] = currValue;
  break;
  case INST_CALL:
    if (inst->operand >= vm->program.number_of_instructions) return INVALID_CALL;
    if (vm->callstack_index >= STACK_MAX_SIZE) return CALL_STACK_OVERFLOW;
    vm->ip++;
    vm->callstack[vm->callstack_index++] = vm->ip;
    vm->ip = inst->operand;
    return OK;
  break;
  case INST_RETURN:
    if (vm->callstack_index < 1) return CALL_STACK_UNDERFLOW;
    uint8_t return_to_position = vm->callstack[vm->callstack_index - 1];
    /**
     * @todo João, avaliar melhor a forma como gerencio o endereço de retorno,
     * quando for a última instrução o endereço armazenado pelo comando CALL
     * será um endereço inválido, uma céula a mais do último endereço válido.
     * Por hora decidi apenas comentar a validação nesse ponto, mas posso 
     * também reformular a forma de armazenar e incrementar o ponteiro de 
     * retorno para incrementar apenas no momento do retorno. Se não causar
     * problemas posso manter assim.
     */
    // if (return_to_position >= vm->program.number_of_instructions) return INVALID_CALL;
    vm->ip = return_to_position;
    vm->callstack_index--;
    return OK;
  break;
  default:
    return INVALID_INSTRUCTION;
  }

  vm->ip++;

  return OK;
}

const char *signal_to_name(enum signals signal)
{
  switch(signal) {
  case OK: return "OK";
  case INVALID_INSTRUCTION: return "INVALID_INSTRUCTION";
  case STACK_OVERFLOW: return "STACK_OVERFLOW";
  case STACK_UNDERFLOW: return "STACK_UNDERFLOW";
  case INVALID_JUMP_POSITION: return "INVALID_JUMP_POSITION";
  case INVALID_OPERAND: return "INVALID_OPERAND";
  case INVALID_CALL: return "INVALID_CALL";
  case CALL_STACK_OVERFLOW: return "CALL_STACK_OVERFLOW";
  case CALL_STACK_UNDERFLOW: return "CALL_STACK_UNDERFLOW";
  }
  assert(0 || "Unreacheable");
  return "";
}

typedef struct execution_configuration
{
  bool is_limited;
  size_t max_execution_ticks;
} execution_configuration_t;

void execute_program(vm_instance_t *vm, execution_configuration_t *configuration)
{
  while (vm->ip < vm->program.number_of_instructions)
  {
    vm->tick_count++;
    enum signals signal = execute_inst(vm, &vm->program.instructions[vm->ip]);

    if (signal != OK) {
      // @todo João, tentar trazer mais dados do sinal, talvez isso apenas
      // em modo verboso
      printf("Execução interrompida erro %s\n", signal_to_name(signal));
      break;
    }

    if (vm->halted) {
      break;
    }

    if (configuration->is_limited && vm->tick_count >= configuration->max_execution_ticks)
    {
      break;
    }
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

program_t read_program_from_file(const char *file_path)
{
  FILE *fd = fopen(file_path, "rb");
  if (fd == NULL) {
    fprintf(stderr, "erro abrindo o arquivo: %s", file_path);
    exit(EXIT_FAILURE);
  }
  fseek(fd, 0, SEEK_END);
  long file_size = ftell(fd);
  fseek(fd, 0, SEEK_SET); // Esqueci de rebobinar

  if (!(file_size % sizeof(inst_t) == 0)) {
    fprintf(stderr, "Número de bytes não bate : %s %d", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }
  void *buffer = malloc(file_size);
  fread(buffer, file_size, 1, fd);

  fclose(fd);

  return (program_t) {
    .instructions = (inst_t *) buffer,
    .number_of_instructions = file_size / sizeof(inst_t),
  };
}

#endif // VM_H
