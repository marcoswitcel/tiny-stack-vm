#include <stdio.h>
#include <stdlib.h>

#include "./assembler.c"

typedef struct command_line_args {
  bool verbose;
  bool dump_instructions;
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
  command_line_args.dump_instructions = is_string_present_in_argv("--dump-instructions", argc, argv);

  if (argc < 2) {
    printf("Forma de uso:\nassembler <caminho-para-o-programa>\n");
    return EXIT_SUCCESS;
  }

  if (command_line_args.verbose) 
  {
    printf("---- Carregando arquivo -----\n");
    printf("[ %s ]\n", argv[1]);
  }
  const char *asm_file_content = read_file_as_cstring(argv[1]);

  stack_instruction_line_t stack;
  stack_instruction_line_init(&stack);

  bool parsed = parse_source_to_instruction_line_stack(asm_file_content, &stack);

  if (!parsed) {
    printf("Programa não parseado com sucesso.\n");
    return EXIT_FAILURE;
  }

  if (command_line_args.dump_instructions) 
  {
    printf("Parseado %d instruções\n", stack.count);
    for (size_t i = 0; i < stack.count; i++)
    {
      print_instruction_line(&stack.store[i]);
    }
  }

  const char *output_file_name = "out.out";
  if (command_line_args.verbose) 
  {
    printf("Programa parseado com sucesso.\n");
    printf("Gerando arquivo: %s\n", output_file_name);
  }

  write_program_out(output_file_name, &stack);

  return EXIT_SUCCESS;
}
