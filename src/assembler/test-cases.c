#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./assembler.c"

void test_skip_whitespace()
{
  const char *text = "  . texto \n nome   \n \n";

  parsing_context_t parsing_context = {
    .source = text,
    .currentIndex = 0,
    .source_length = strlen(text),
  };

  skip_whitespace(&parsing_context);
  assert(parsing_context.currentIndex == 2 && "Deveria avançar até o primeiro caractere");

  parsing_context.currentIndex = 2;
  skip_whitespace(&parsing_context);
  assert(parsing_context.currentIndex == 2 && "Não deveria avançar se não estiver em um caractere 'whitespace'");

  parsing_context.currentIndex = 3;
  skip_whitespace(&parsing_context);
  assert(parsing_context.currentIndex == 4 && "Deveria avançar até o primeiro caractere");

  parsing_context.currentIndex = 9;
  skip_whitespace(&parsing_context);
  assert(parsing_context.currentIndex == 12 && "Deveria avançar até o primeiro caractere");

  parsing_context.currentIndex = 16;
  skip_whitespace(&parsing_context);
  assert(parsing_context.currentIndex == parsing_context.source_length && "Deveria avançar até o primeiro caractere");
}

void test_parseando_um_programa_simples()
{
  const char *text = "  .teste PUSH 25\n PUSH   50\nPLUS 0\n";

  parsing_context_t parsing_context = {
    .source = text,
    .currentIndex = 0,
    .source_length = strlen(text),
  };

  skip_whitespace(&parsing_context);

  assert(parsing_context.currentIndex == 2 && "Deveria estar no index dois");

  maybe_instruction_line_t maybe_instruction_line = parse_instruction_line(&parsing_context);

  assert(parsing_context.currentIndex == 16 && "Deveria estar no index dezesseis");

  if (maybe_instruction_line.matched)
  {
    printf(
        "\nparsed symbol: %s, type(opcode): %d, operand: %d\n",
        maybe_instruction_line.value.label,
        maybe_instruction_line.value.instruction.type,
        maybe_instruction_line.value.instruction.operand);
  }

  while (parsing_context.currentIndex < parsing_context.source_length)
  {
    maybe_instruction_line_t maybe_instruction_line = parse_instruction_line(&parsing_context);

    if (maybe_instruction_line.matched)
    {
      printf(
          "\nparsed symbol: %s, type(opcode): %d, operand: %d\n",
          maybe_instruction_line.value.label,
          maybe_instruction_line.value.instruction.type,
          maybe_instruction_line.value.instruction.operand);
    }
    else
    {
      printf("falha: [%s]", maybe_instruction_line.error_message);
      return;
    }

    // Necessário para empurrar o cursor até o final do linha caso ela tenha acabado
    skip_whitespace(&parsing_context);
  }
}

void test_number_literal_as_number(void)
{
  assert(number_literal_as_number("0") == 0 && "Parasenado corretamente");
  assert(number_literal_as_number("128") == 128 && "Parasenado corretamente");
  assert(number_literal_as_number("52") == 52 && "Parasenado corretamente");
  assert(number_literal_as_number("255") == 255 && "Parasenado corretamente");
  assert(number_literal_as_number("256") < 0 && "Parasenado corretamente");
}

void test_char_literal_as_number(void)
{
  assert(char_literal_as_number("a") == 'a' && "Parasenado corretamente");
  assert(char_literal_as_number("B") == 'B' && "Parasenado corretamente");
  assert(char_literal_as_number("\\n") == '\n' && "Parasenado corretamente");
}

void test_cases(void)
{
  test_parseando_um_programa_simples();
  // Teste de subfuncionalidades
  test_number_literal_as_number();
  test_char_literal_as_number();
  test_skip_whitespace();
}

int main(void)
{
  test_cases();

  return EXIT_SUCCESS;
}
