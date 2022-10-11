#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./vm.c"
#include "./compiler-requirements.h"

typedef struct command_line_args {
  bool verbose;
  bool dump_stack;
  bool limit_instruction;
  size_t max_ticks;
} command_line_arguments_t;


int main(int argc, const char *argv[])
{
  // @todo João, fazer as funções que consomem argumentos
  command_line_arguments_t command_line_args = {0};

  // @todo João melhorar forma de consumir parâmetros e principalmente
  // adicionar parâmetros para acionar ou não funcionalidade abaixo
  if (argc < 2) {
    printf("Forma de uso:\nvm <caminho-para-o-programa>\n");
    return EXIT_SUCCESS;
  }

  // @todo João, deixar esse print apenas em modo verboso
  if (command_line_args.verbose) 
  {
    printf("---- LOADED program -----\n");
  }
  vm_instance_t vm = {0};
  vm.program = read_program_from_file(argv[1]);

  // @todo João, num futuro próximo remover o limite de ticks, deixar ele
  // parametrizável
  execute_program(&vm);

  // @todo João, deixar esse dump apenas quando requisitado através de uma flag
  if (command_line_args.dump_stack) 
  {
    dump_stack_memory(&vm);
  }

  return EXIT_SUCCESS;
}
