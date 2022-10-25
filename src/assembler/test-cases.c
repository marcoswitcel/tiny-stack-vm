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

void test_parse_number(void)
{
  const char *source = "    155 0 23b 255 256 12a";

  parsing_context_t parsing_context = {
    .source = source,
    .currentIndex = 0,
    .source_length = strlen(source),
  };

  {
    parsing_context.currentIndex = 0;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok == false && "Não deveria parsear nada");
    assert(result.error_message != NULL && strlen(result.error_message) && "A mensagem de erro deve ser uma string não vazia");
  }

  {
    parsing_context.currentIndex = 4;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok && "Deveria parsear corretamente");
    assert(result.literal_form != NULL && strlen(result.literal_form) && "A forma literal deve ser uma string não vazia");
    assert(result.number == 155 && "O número esperado era");
  }

  {
    parsing_context.currentIndex = 5;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok && "Deveria parsear corretamente");
    assert(result.literal_form != NULL && strlen(result.literal_form) && "A forma literal deve ser uma string não vazia");
    assert(result.number == 55 && "O número esperado era");
  }

  {
    parsing_context.currentIndex = 6;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok && "Deveria parsear corretamente");
    assert(result.literal_form != NULL && strlen(result.literal_form) && "A forma literal deve ser uma string não vazia");
    assert(result.number == 5 && "O número esperado era");
  }

  {
    parsing_context.currentIndex = 8;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok && "Deveria parsear corretamente");
    assert(result.literal_form != NULL && strlen(result.literal_form) && "A forma literal deve ser uma string não vazia");
    assert(result.number == 0 && "O número esperado era");
  }

  {
    parsing_context.currentIndex = 10;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok == false && "Não deveria parsear nada");
    assert(result.error_message != NULL && strlen(result.error_message) && "A mensagem de erro deve ser uma string não vazia");
  }

  {
    parsing_context.currentIndex = 14;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok && "Deveria parsear corretamente");
    assert(result.literal_form != NULL && strlen(result.literal_form) && "A forma literal deve ser uma string não vazia");
    assert(result.number == 255 && "O número esperado era");
  }

  {
    parsing_context.currentIndex = 18;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok == false && "Não deveria parsear nada");
    assert(result.error_message != NULL && strlen(result.error_message) && "A mensagem de erro deve ser uma string não vazia");
  }

  {
    parsing_context.currentIndex = 22;
    maybe_parsed_number_t result = parse_number(&parsing_context);

    assert(result.ok == false && "Não deveria parsear nada");
    assert(result.error_message != NULL && strlen(result.error_message) && "A mensagem de erro deve ser uma string não vazia");
  }
}

void test_parse_char(void)
{
  const char *source = "    'a' '\n' '\\n' 'Z'";

  parsing_context_t parsing_context = {
    .source = source,
    .currentIndex = 0,
    .source_length = strlen(source),
  };

  {
    parsing_context.currentIndex = 0;
    maybe_parsed_number_t result = parse_char(&parsing_context);

    assert(result.ok == false && "Não deveria parsear nada");
    assert(result.error_message != NULL && strlen(result.error_message) && "A mensagem de erro deve ser uma string não vazia");
  }

  {
    parsing_context.currentIndex = 4;
    maybe_parsed_number_t result = parse_char(&parsing_context);

    assert(result.ok && "Deveria parsear corretamente");
    assert(result.literal_form != NULL && strlen(result.literal_form) && "A forma literal deve ser uma string não vazia");
    assert(result.number == 'a' && "O número esperado era");
  }

  {
    parsing_context.currentIndex = 8;
    maybe_parsed_number_t result = parse_char(&parsing_context);

    assert(result.ok == false && "Não deveria parsear nada");
    assert(result.error_message != NULL && strlen(result.error_message) && "A mensagem de erro deve ser uma string não vazia");
  }

  {
    parsing_context.currentIndex = 12;
    maybe_parsed_number_t result = parse_char(&parsing_context);

    assert(result.ok && "Deveria parsear corretamente");
    assert(result.literal_form != NULL && strlen(result.literal_form) && "A forma literal deve ser uma string não vazia");
    assert(result.number == '\n' && "O número esperado era");
  }

  {
    parsing_context.currentIndex = 17;
    maybe_parsed_number_t result = parse_char(&parsing_context);

    assert(result.ok && "Deveria parsear corretamente");
    assert(result.literal_form != NULL && strlen(result.literal_form) && "A forma literal deve ser uma string não vazia");
    assert(result.number == 'Z' && "O número esperado era");
  }
}

void test_parseando_um_programa_simples()
{
  const char *text = "  .teste PUSH 25\n PUSH   50\nPLUS 0 'o' 'l' PUSH 'a' ' ' \n";

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
  test_parse_number();
  test_parse_char();
}

int main(void)
{
  test_cases();

  return EXIT_SUCCESS;
}
