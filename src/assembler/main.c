#include <stdio.h>
#include <stdlib.h>

#include "./assembler.c"


int main(int argc, const char *argv[])
{

  if (argc < 2) {
    printf("Forma de uso:\nassembler <caminho-para-o-programa>\n");
    return EXIT_SUCCESS;
  }

  printf("---- LOADED program -----\n");
  const char *asm_file_content = read_file_as_cstring(argv[1]);

  stack_instruction_line_t stack;
  stack_instruction_line_init(&stack);

  bool parsed = parse_source_to_instruction_line_stack(asm_file_content, &stack);

  if (!parsed) {
    return EXIT_FAILURE;
  }

  printf("Parseado %d instruções\n", stack.count);
  for (size_t i = 0; i < stack.count; i++)
  {
    print_instruction_line(&stack.store[i]);
  }

  printf("Programa parseado com sucesso.\n");

  const char *output_file_name = "out.out";
  printf("Gerando arquivo: %s\n", output_file_name);

  write_program_out(output_file_name, &stack);

  return EXIT_SUCCESS;
}
