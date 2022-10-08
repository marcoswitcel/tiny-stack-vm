#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../vm-definitions.h"
#include "../compiler-requirements.h"

typedef struct parsing_context
{
  const char *source;
  const size_t source_length;
  size_t currentIndex;
} parsing_context_t;

typedef struct maybe_instruction
{
  bool matched;
  inst_t instruction;
  parsing_context_t new_parsing_context;
} maybe_instruction_t;

/**
 * @brief Retorna o endereço do próximo char a ser consumido
 * 
 * @param parsing_context 
 * @return const char* 
 */
static inline const char* get_current_address(const parsing_context_t *parsing_context)
{
  return parsing_context->source + parsing_context->currentIndex;
}

// @todo João, revisar o tamanho máximo a memória de instruções
// static inst_t instructions[STACK_MAX_SIZE] = {0};

static inline bool is_whitespace(char value)
{
  return (value == ' ' || value == '\t' || value == '\r' || value == '\n');
}

static inline bool is_digit(char value)
{
  return value > ('0' - 1) && value < ('9' + 1);
}

static inline bool is_digit_not_zero(char value)
{
  return value > '0' && value < ('9' + 1);
}

void skip_whitespace(parsing_context_t *parsing_context)
{
  const char *source = get_current_address(parsing_context);
  char value;

  while ((value = *source) && is_whitespace(value))
  {
    source++;
  }

  parsing_context->currentIndex += source - get_current_address(parsing_context);
}

typedef struct optional_intruction 
{
  bool is;
  instructions_t inst;
} optional_instruction_t;

/**
 * @brief Valida se o símbolo provido é um nome de instrução conhecido
 * @todo João, terminar de implementar, essa primeira implementação serve
 * apenas para testar e está incompleta 
 * 
 * @param operation_name 
 * @return optional_instruction_t 
 */
optional_instruction_t lookup_instruction_for_symbol(const char *operation_name)
{
  optional_instruction_t result = {0};

  if (!strcmp(operation_name, "PUSH")) {
    result.is = true;
    result.inst = INST_PUSH;
    return result;
  }

  if (!strcmp(operation_name, "PLUS")) {
    result.is = true;
    result.inst = INST_PLUS;
    return result;
  }

  return result;
}

/**
 * @brief Valida se o símbolo provido é um nome de instrução conhecido
 * @todo João, ajustar dependência dessa função na função de lookup
 * 
 * @param operation_name 
 * @return true 
 * @return false 
 */
bool is_a_valid_operation_name(const char *operation_name)
{
  return lookup_instruction_for_symbol(operation_name).is;
}

typedef struct instruction_line
{
  const char *label;
  inst_t instruction;
} instruction_line_t;

typedef struct maybe_instruction_line
{
  bool matched;
  instruction_line_t value;
  const char *error_message;
} maybe_instruction_line_t;

typedef struct maybe_parsed
{
  bool ok;
  const char *symbol;
  const char *error_message;
} maybe_parsed_t;

maybe_parsed_t parse_symbol(const parsing_context_t *parsing_context)
{
  maybe_parsed_t maybe = {0};
  const char *source = get_current_address(parsing_context);
  char current_value;

  while ((current_value = *source) && ((current_value >= 'a' && 'z' >= current_value) ||
                                       (current_value >= 'A' && 'Z' >= current_value)))
  {
    source++;
  }

  size_t symbol_size = source - get_current_address(parsing_context);

  /**
   * @note João, por hora checo a variável `symbol_size` para saber se o
   * parser consumiu pelo menos um token, senão reporto o erro. Os outros
   * dois checks do if são pra confirmar que os tokens a seguir são tokens
   * válidos para a terminação do símbolo.
   */
  if (symbol_size && (current_value == '\0' || is_whitespace(current_value)))
  {
    char *symbol = malloc(sizeof(char) * symbol_size + 1);
    memcpy(symbol, get_current_address(parsing_context), symbol_size);
    symbol[symbol_size] = '\0';
    maybe.ok = true;
    maybe.symbol = symbol;
  }
  else
  {
    maybe.ok = false;
    maybe.symbol = ".Symbol error";
  }

  return maybe;
}

/**
 * @brief Converte um `number literal` para um int16_t, qualquer número
 * negativo representa o fracasso no processo de conversão.
 *
 * @param number_literal_form
 * @return int16_t
 */
int16_t number_literal_as_number(const char *number_literal_form)
{
  const size_t number_literal_form_length = strlen(number_literal_form);
  size_t size = number_literal_form_length;

  if (size > 3)
    return -1;

  int16_t number = 0;
  while (size--)
  {
    // @todo João, aqui sempre deve ser um dígito válido, mas deixo o
    // assert para validar isso durante o desenvolvimento, se essa
    // condição for quebrada recorrentemente, pode ser mudado para um
    // check em tempo de execução
    assert(is_digit(number_literal_form[size]) && "Deve ser um dígito válido");
    number += (number_literal_form[size] - '0') * pow(10, (number_literal_form_length - 1) - size);
  }

  if (number > UINT8_MAX)
    return -1;

  return number;
}

typedef struct maybe_parsed_number
{
  bool ok;
  uint8_t number;
  const char *literal_form;
  const char *error_message;
} maybe_parsed_number_t;

maybe_parsed_number_t parse_number(const parsing_context_t *parsing_context)
{
  maybe_parsed_number_t maybe_number = {0};

  const char *source = get_current_address(parsing_context);
  char current_value = *source;

  if (is_digit_not_zero(current_value))
  {

    while ((current_value = *source) && is_digit(current_value))
    {
      source++;
    }

    size_t literal_form_size = source - get_current_address(parsing_context);

    if (current_value == '\0' || is_whitespace(current_value))
    {
      char *literal_form = malloc(sizeof(char) * literal_form_size + 1);
      memcpy(literal_form, get_current_address(parsing_context), literal_form_size);
      literal_form[literal_form_size] = '\0';

      int16_t number = number_literal_as_number(literal_form);

      if (number < 0)
      {
        maybe_number.ok = false;
        maybe_number.error_message = "Numero invalido";
        free(literal_form);
      }
      else
      {
        maybe_number.ok = true;
        maybe_number.number = (uint8_t)number;
        maybe_number.literal_form = literal_form;
      }
    }
    else
    {
      maybe_number.ok = false;
      maybe_number.error_message = "Numero invalido";
    }
  } else if (current_value == '0') {
    source++;
    current_value = *source;
    if (current_value == '\0' || is_whitespace(current_value))
    {
      maybe_number.ok = true;
      maybe_number.number = 0;
      maybe_number.literal_form = "0";
    }
    else
    {
      maybe_number.ok = false;
      maybe_number.error_message = "Numero invalido, nao pode começar com zero";
    }
  }

  return maybe_number;
}

/**
 * @brief Parseia um linha lógica. A linha é formada por uma etiqueta/marcação,
 * opcional, seguida por um nome de instrução
 *
 * @param parsing_context
 */
maybe_instruction_line_t parse_instruction_line(parsing_context_t *parsing_context)
{
  maybe_instruction_line_t maybe_instruction_line = {0};
  maybe_instruction_line.error_message = "Mensagem padrão";
  skip_whitespace(parsing_context);
  const char *source = get_current_address(parsing_context);
  char current_value = *source;

  if (current_value == '.')
  {
    parsing_context_t new_parsing_context = *parsing_context;
    new_parsing_context.currentIndex++;
    maybe_parsed_t maybe_parsed = parse_symbol(&new_parsing_context);

    if (maybe_parsed.ok)
    {
      maybe_instruction_line.value.label = maybe_parsed.symbol;
      parsing_context->currentIndex += strlen(maybe_instruction_line.value.label) + 1;
    }
    else
    {
      maybe_instruction_line.error_message = "Não foi possível parsear o label";
      maybe_instruction_line.matched = false;
      return maybe_instruction_line;
    }

    skip_whitespace(parsing_context);

    source = get_current_address(parsing_context);
    current_value = *source;
  }

  if (is_digit(current_value))
  {
    maybe_parsed_number_t maybe_number = parse_number(parsing_context);

    if (maybe_number.ok)
    {
      maybe_instruction_line.value.instruction = (inst_t){
        .type = INST_PUSH,
        .operand = maybe_number.number,
      };
      parsing_context->currentIndex += strlen(maybe_number.literal_form);
    }
    else
    {
      maybe_instruction_line.error_message = "Esperava um número (push)";
      maybe_instruction_line.matched = false;
      return maybe_instruction_line;
    }
  }
  else
  {
    maybe_parsed_t maybe_parsed = parse_symbol(parsing_context);

    if (maybe_parsed.ok && is_a_valid_operation_name(maybe_parsed.symbol))
    {
      instructions_t inst = lookup_instruction_for_symbol(maybe_parsed.symbol).inst; 
      maybe_instruction_line.value.instruction = (inst_t) {
        .type = inst,
        .operand = 0,
      };
      parsing_context->currentIndex += strlen(maybe_parsed.symbol);
    }
    else
    {
      // @todo João, temporário
      maybe_instruction_line.error_message = "Esperava uma instrução valida";
      maybe_instruction_line.matched = false;
      return maybe_instruction_line;
    }

    skip_whitespace(parsing_context);

    maybe_parsed_number_t maybe_number = parse_number(parsing_context);

    if (maybe_number.ok)
    {
      maybe_instruction_line.value.instruction.operand = maybe_number.number;
      parsing_context->currentIndex += strlen(maybe_number.literal_form);
    }
    else
    {
      maybe_instruction_line.error_message = "Esperava um número";
      maybe_instruction_line.matched = false;
      return maybe_instruction_line;
    }

    // @todo João, skip e atualização do index do parsing context, ainda precisamos parsear o operand
    // @todo João, avaliar as regras semânticas entram em ação aqui, algumas instruções tem comandos
    // parâmetro opcional e parâmetro padrão diferente de 0
  }
  maybe_instruction_line.matched = true;
  return maybe_instruction_line;
}

/**
 * @brief Recebe o caminho de um arquivo e lê seu conteúdo como se fosse uma
 * string terminada com 'NULL'
 * 
 * @param file_path Caminho do arquivo
 * @return const char* 
 */
const char* read_file_as_cstring(const char *file_path)
{
  FILE *fd = fopen(file_path, "rb");
  if (fd == NULL) {
    fprintf(stderr, "erro abrindo o arquivo: %s", file_path);
    exit(EXIT_FAILURE);
  }
  fseek(fd, 0, SEEK_END);
  long file_size = ftell(fd);
  fseek(fd, 0, SEEK_SET); // Esqueci de rebobinar

  void *buffer = malloc(file_size);
  if (buffer == NULL) {
    fclose(fd);
    fprintf(stderr, "não conseguiu alocar memória para o arquivo");
    exit(EXIT_FAILURE);
  }
  
  fread(buffer, file_size, 1, fd);
  fclose(fd);

  return (const char*) buffer;
}

/**
 * @brief Parseia e printa as informações extraídas do source asm
 * @todo João, temporário isso aqui
 * 
 * @param source 
 * @return true 
 * @return false 
 */
bool parse_and_print(const char *source)
{
  parsing_context_t parsing_context = {
    .source = source,
    .currentIndex = 0,
    .source_length = strlen(source),
  };
  
  while (parsing_context.currentIndex < parsing_context.source_length)
  {
    maybe_instruction_line_t maybe_instruction_line = parse_instruction_line(&parsing_context);

    if (maybe_instruction_line.matched)
    {
      if (maybe_instruction_line.value.label) {
        printf(
          "{ .label =  \"%s\", .opcode = %d, .operand = %d }\n",
          maybe_instruction_line.value.label,
          maybe_instruction_line.value.instruction.type,
          maybe_instruction_line.value.instruction.operand
        );
      } else {
        printf(
          "{ .label =  NULL, .opcode = %d, .operand = %d }\n",
          maybe_instruction_line.value.instruction.type,
          maybe_instruction_line.value.instruction.operand
        );
      }
      
    }
    else
    {
      printf(">>>> falha: [%s] <<<<", maybe_instruction_line.error_message);
      return false;
    }

    // Necessário para empurrar o cursor até o final do linha caso ela tenha acabado
    skip_whitespace(&parsing_context);
  }

  printf("Programa parseado com sucesso.\n");

  return true;
}
