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

  printf("Conteúdo do arquivo:\n---------------------\n%s\n---------------------\n", asm_file_content);

  return EXIT_SUCCESS;
}
