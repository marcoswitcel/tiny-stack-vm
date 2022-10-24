#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./vm.c"
#include "../compiler-requirements.h"

typedef struct command_line_args {
  bool verbose;
  bool dump_stack;
  bool limit_instruction;
  bool execution_info;
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
  // @todo João, continuar melhorando a forma de consumir argumentos
  command_line_arguments_t command_line_args = {0};

  command_line_args.verbose = is_string_present_in_argv("--verbose", argc, argv);
  command_line_args.dump_stack = is_string_present_in_argv("--dump-stack", argc, argv);
  command_line_args.execution_info = is_string_present_in_argv("--execution-info", argc, argv);
  command_line_args.limit_instruction = is_string_present_in_argv("--limit-ticks", argc, argv); // @todo João, permitir passar um inteiro para customizar o número de instruções limite

  if (argc < 2) {
    printf("Forma de uso:\nvm <caminho-para-o-programa>\n");
    return EXIT_SUCCESS;
  }

  if (command_line_args.verbose) 
  {
    printf("---- Carregando programa -----\n");
    printf("[ %s ]\n", argv[1]);
  }
  vm_instance_t vm = {0};
  vm.program = read_program_from_file(argv[1]);

  execution_configuration_t configuration = {0};

  if (command_line_args.limit_instruction)
  {
    configuration.is_limited = true;
    // @todo João, parsear valor da linha de comando
    configuration.max_execution_ticks = 255;
  }

  execute_program(&vm, &configuration);

  if (command_line_args.execution_info)
  {
    printf("Número de ticks executados: %ld\n", vm.tick_count);
  }

  if (command_line_args.dump_stack) 
  {
    dump_stack_memory(&vm);
  }

  return EXIT_SUCCESS;
}
