#ifndef VM_DEFINITIONS_H
#define VM_DEFINITIONS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
  INST_GREATER_THAN,
  INST_LOWER_THAN,
  INST_HALT,
  INST_WRITE,
  INST_SWAP,
} instructions_t;

enum signals {
  OK,
  INVALID_INSTRUCTION,
  STACK_OVERFLOW,
  STACK_UNDERFLOW,
  INVALID_JUMP_POSITION,
  INVALID_OPERAND,
};

typedef struct inst {
  instructions_t type;
  uint8_t operand;
} inst_t;

typedef struct program {
  inst_t *instructions;
  size_t number_of_instructions;
} program_t;

typedef struct vm_instance {
  word_t stack[STACK_MAX_SIZE];
  uint16_t index;
  size_t ip;
  program_t program;
  bool halted;
} vm_instance_t;

#endif // VM_DEFINITIONS_H
