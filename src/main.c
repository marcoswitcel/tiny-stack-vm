#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./vm.c"
#include "./compiler-requirements.h"


int main(int argc, const char *argv[])
{

  if (argc < 2) {
    printf("Forma de uso:\nvm <caminho-para-o-programa>\n");
    return EXIT_SUCCESS;
  }

  printf("---- LOADED program -----\n");
  vm_instance_t vm = {0};
  vm.program = read_program_from_file(argv[1]);

  execute_program(&vm);
  dump_stack_memory(&vm);

  return EXIT_SUCCESS;
}
