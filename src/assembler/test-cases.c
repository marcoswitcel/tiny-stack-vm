#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "./assembler.c"

void test01()
{
  // Primeiro exemplo a ser parseado
  const char *text = "PUSH 25\n PUSH   50\nPLUS\n";

  parsing_context_t initial_parsing_context = {
      .source = text,
      .currentIndex = 0,
  };

  // parser (ctx) -> ok|err
  maybe_instruction_t maybe = parse("PUSH", initial_parsing_context);

  printf("Parsing context inicial: currenIndex %ld", initial_parsing_context.currentIndex);

  if (maybe.matched)
  {
    printf("deu matche: currenIndex %ld", maybe.new_parsing_context.currentIndex);
  }
  else
  {
    printf("não de matche: currenIndex %ld", maybe.new_parsing_context.currentIndex);
  }
}

void test02()
{
  // static inst_t instructions[STACK_MAX_SIZE] = {0};
  // size_t instructions_size = 0;

  // Primeiro exemplo a ser parseado
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
        maybe_instruction_line.label,
        maybe_instruction_line.instruction.type,
        maybe_instruction_line.instruction.operand);
  }

  while (parsing_context.currentIndex < parsing_context.source_length)
  {
    maybe_instruction_line_t maybe_instruction_line = parse_instruction_line(&parsing_context);

    if (maybe_instruction_line.matched)
    {
      printf(
          "\nparsed symbol: %s, type(opcode): %d, operand: %d\n",
          maybe_instruction_line.label,
          maybe_instruction_line.instruction.type,
          maybe_instruction_line.instruction.operand);
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

void test_cases(void)
{
  test01();
  test02();
  // Teste de subfuncionalidades
  test_number_literal_as_number();
}

int main(void)
{
  test_cases();

  return EXIT_SUCCESS;
}
