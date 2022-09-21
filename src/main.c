#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./vm.c"

int main(int argc, const char *argv[])
{
  static_assert(sizeof(instructions_t) == 1, "Enums de 1 byte com -fshort-enums não habilitado");

  if (argc < 2) {
    printf("Forma de uso:\nvm <caminho-para-o-programa>\n");
  }

  printf("---- LOADED program -----\n");
  vm_instance_t vm = {0};
  vm.program = read_program_from_file(argv[1]);

  execute_program(&vm);
  dump_stack_memory(&vm);

  return EXIT_SUCCESS;
}
