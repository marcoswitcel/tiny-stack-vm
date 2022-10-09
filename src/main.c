#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./vm.c"
#include "./compiler-requirements.h"


int main(int argc, const char *argv[])
{

  // @todo João melhorar forma de consumir parâmetros e principalmente
  // adicionar parâmetros para acionar ou não funcionalidade abaixo
  if (argc < 2) {
    printf("Forma de uso:\nvm <caminho-para-o-programa>\n");
    return EXIT_SUCCESS;
  }

  // @todo João, deixar esse print apenas em modo verboso
  printf("---- LOADED program -----\n");
  vm_instance_t vm = {0};
  vm.program = read_program_from_file(argv[1]);

  // @todo João, num futuro próximo remover o limite de ticks, deixar ele
  // parametrizável
  execute_program(&vm);

  // @todo João, deixar esse dump apenas quando requisitado através de uma flag
  dump_stack_memory(&vm);

  return EXIT_SUCCESS;
}
