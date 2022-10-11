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

bool is_string_present_in_argv(const char *switch_name, int argc, const char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    if (!strcmp(argv[i], switch_name))
    {
      return true;
    }
  }
  return false;
}


int main(int argc, const char *argv[])
{
  // @todo João, fazer as funções que consomem argumentos
  command_line_arguments_t command_line_args = {0};

  command_line_args.verbose = is_string_present_in_argv("--verbose", argc, argv);
  command_line_args.dump_stack = is_string_present_in_argv("--dump-stack", argc, argv);

  // @todo João melhorar forma de consumir parâmetros e principalmente
  // adicionar parâmetros para acionar ou não funcionalidade abaixo
  if (argc < 2) {
    printf("Forma de uso:\nvm <caminho-para-o-programa>\n");
    return EXIT_SUCCESS;
  }

  // @todo João, deixar esse print apenas em modo verboso
  if (command_line_args.verbose) 
  {
    printf("---- Carregando programa -----\n");
    printf("[ %s ]\n", argv[1]);
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
