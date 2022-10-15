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
 * Resolvi anotar aqui alguns consensos que cheguei a respeito das instruções:
 * 
 * -- Algumas instruções não fazem uso do valor do operador, porém, devem ser 
 * normalizadas para receber um zero como valor do operador.
 * -- Algumas instruções como o swap terão um valor padrão, o que quer dizer
 * que na forma textual não sera necessário explicitar o valor. Exemplo: a
 * instrução swap terá como valor padrão o número 1, porém, ela aceita valores
 * de 0 à 255, embora se eles são validos ou não depende do tamanho da stack em
 * runtime, e o valor zero em particular sempre será uma `noop` em termos de
 * efeito na aplicação.
 * @todo João, implementar um operador de incremento, provavelmente através do
 * comando plus (usando operand diferente de zero para isso). Uma vez implemen-
 * tado, implementar também o decremento.
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
  INST_CALL,
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

#define INSTRUCTION_NAMES_LIST(DO)\
  DO(PLUS) \
  DO(SUB) \
  DO(PUSH) \
  DO(JUMP) \
  DO(CALL) \
  DO(DUP) \
  DO(POP) \
  DO(JUMP_NOT_ZERO) \
  DO(JUMP_ZERO) \
  DO(GREATER_THAN) \
  DO(LOWER_THAN) \
  DO(HALT) \
  DO(WRITE) \
  DO(SWAP)

enum signals {
  OK,
  INVALID_INSTRUCTION,
  INVALID_CALL,
  STACK_OVERFLOW,
  STACK_UNDERFLOW,
  CALL_STACK_OVERFLOW,
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
  size_t tick_count;
  uint8_t callstack[STACK_MAX_SIZE];
  uint8_t callstack_index;
} vm_instance_t;

#endif // VM_DEFINITIONS_H
