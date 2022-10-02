#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "./vm-definitions.h"
#include "./compiler-requirements.h"

typedef struct parsing_context {
    const char *source;
    const size_t source_length;
    size_t currentIndex;
} parsing_context_t;

typedef struct maybe_instruction {
    bool matched;
    inst_t instruction;
    parsing_context_t new_parsing_context;
} maybe_instruction_t;

// @todo João, revisar o tamanho máximo a memória de instruções
// static inst_t instructions[STACK_MAX_SIZE] = {0};

static maybe_instruction_t parse(const char *token, parsing_context_t parsing_context)
{
    const char *sourceCodeStart = parsing_context.source+parsing_context.currentIndex;
    const char *tokenStart = token;
    size_t consumed_bytes = 0;

    assert(*token != '\0' && "Token não devem ser strings vazias");

    while (*sourceCodeStart == *tokenStart && *sourceCodeStart != '\0')
    {
        sourceCodeStart++;
        tokenStart++;
        consumed_bytes++;
    }

    if (consumed_bytes == strlen(token)) {
        return (maybe_instruction_t) {
            .matched = true,
            .instruction = {
                .type = INST_PUSH,
                .operand = 0,
            },
            .new_parsing_context = {
                .source = parsing_context.source + consumed_bytes,
                .currentIndex = parsing_context.currentIndex + consumed_bytes,
            }
        };
    }

    return (maybe_instruction_t) {
        .matched = false,
        .instruction = {0},
        .new_parsing_context = parsing_context
    };
}

void test01()
{
    // Primeiro exemplo a ser parseado
    const char * text = "PUSH 25\n PUSH   50\nPLUS\n";

    parsing_context_t initial_parsing_context = {
        .source = text,
        .currentIndex = 0,
    };

    // parser (ctx) -> ok|err
    maybe_instruction_t maybe = parse("PUSH", initial_parsing_context);

    printf("Parsing context inicial: currenIndex %ld", initial_parsing_context.currentIndex);

    if (maybe.matched) {
        printf("deu matche: currenIndex %ld", maybe.new_parsing_context.currentIndex);
    } else {
        printf("não de matche: currenIndex %ld", maybe.new_parsing_context.currentIndex);
    }

    
}

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
    const char *source = parsing_context->source + parsing_context->currentIndex;
    char value; 

    while ((value = *source) && is_whitespace(value))
    {
        source++;
    }

    parsing_context->currentIndex += source - (parsing_context->source + parsing_context->currentIndex);
}

typedef struct maybe_instruction_line {
    bool matched;
    const char *label;
    inst_t instruction;
} maybe_instruction_line_t;

typedef struct maybe_parsed {
    bool ok;
    const char *symbol;
    const char *error_message;
} maybe_parsed_t;

maybe_parsed_t parse_symbol(const parsing_context_t *parsing_context)
{
    maybe_parsed_t maybe = {0};
    const char *source = parsing_context->source + parsing_context->currentIndex;
    char current_value;

    while ((current_value = *source) && (
        (current_value >= 'a' && 'z' >= current_value) ||
        (current_value >= 'A' && 'Z' >= current_value)
    ))
    {
        source++;
    }

    size_t symbol_size = source - (parsing_context->source + parsing_context->currentIndex);

    if (current_value == '\0' || is_whitespace(current_value) ) {
        char * symbol = malloc(sizeof(char) * symbol_size + 1);
        memcpy(symbol, parsing_context->source + parsing_context->currentIndex, symbol_size);
        symbol[symbol_size] = '\0';
        maybe.ok = true;
        maybe.symbol = symbol;
    } else {
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

    if (size > 3) return -1;

    int16_t number = 0;
    while (size--)
    {
        number += (number_literal_form[size] - '0') * pow(10, (number_literal_form_length - 1) - size);
    }

    if (number > UINT8_MAX) return -1;
    
    return number;
}

typedef struct maybe_parsed_number {
    bool ok;
    uint8_t number;
    const char *literal_form;
    const char *error_message;
} maybe_parsed_number_t;

maybe_parsed_number_t parse_number(const parsing_context_t *parsing_context)
{
    maybe_parsed_number_t maybe_number = {0};

    const char *source = parsing_context->source + parsing_context->currentIndex;
    char current_value = *source;

    if (is_digit_not_zero(current_value)) {

        while ((current_value = *source) && is_digit(current_value))
        {
            source++;
        }

        size_t literal_form_size = source - (parsing_context->source + parsing_context->currentIndex);

        
        if (current_value == '\0' || is_whitespace(current_value) ) {
            char * literal_form = malloc(sizeof(char) * literal_form_size + 1);
            memcpy(literal_form, parsing_context->source + parsing_context->currentIndex, literal_form_size);
            literal_form[literal_form_size] = '\0';

            int16_t number = number_literal_as_number(literal_form);

            if (number < 0) {
                // @todo João, leak da memória referenciada por `literal_form`
                maybe_number.ok = false;
                maybe_number.error_message = "Numero invalido";    
            } else {
                maybe_number.ok = true;
                maybe_number.number = (uint8_t) number;
                maybe_number.literal_form = literal_form;
            }
        } else {
            maybe_number.ok = false;
            maybe_number.error_message = "Numero invalido";
        }
    }

    return maybe_number;
}

/**
 * @brief Parseia um linha lógica. A linha é formada por uma etiqueta/marcação,
 * opcional, seguida por um nome de instrução
 * 
 * @todo João, fazer os ajustes na struct `maybe_instruction_line` para poder
 * retornar mensagens de erro de parsing
 * @todo João, fazer os ajustes necessários na struct `maybe_parsed` para 
 * retornar as mensagens de erro de parsing
 * 
 * @param parsing_context 
 */
maybe_instruction_line_t parse_instruction_line(parsing_context_t *parsing_context)
{
    maybe_instruction_line_t maybe_instruction_line = {0};
    skip_whitespace(parsing_context);
    const char *source = parsing_context->source + parsing_context->currentIndex;
    char current_value = *source;

    // @todo João, checar se está funcionando
    // Tenta parsear um label (opcional)
    if (current_value == '.') {
        parsing_context_t new_parsing_context = *parsing_context;
        new_parsing_context.currentIndex++;
        maybe_parsed_t maybe_parsed = parse_symbol(&new_parsing_context);

        if (maybe_parsed.ok) {
            maybe_instruction_line.label = maybe_parsed.symbol;
            parsing_context->currentIndex += strlen(maybe_instruction_line.label) + 1;
        } else {
            maybe_instruction_line.matched = false;
            goto shoud_return;
        }

        skip_whitespace(parsing_context);

        source = parsing_context->source + parsing_context->currentIndex;
        current_value = *source;
    }

    if (current_value > '0' && current_value < ('9'+1)) { //  @todo João, fazer esse logo em seguida
        // @todo João, pode ser um número (açucar sintático para um push)
    } else {
        maybe_parsed_t maybe_parsed = parse_symbol(parsing_context);

        if (maybe_parsed.ok && !strcmp(maybe_parsed.symbol, "PUSH") ) {
            maybe_instruction_line.instruction = (inst_t) {
                .type = INST_PUSH,
                .operand = 128,
            };
            parsing_context->currentIndex += strlen(maybe_parsed.symbol);
        } else {
            maybe_instruction_line.matched = false;
            goto shoud_return;
        }

        skip_whitespace(parsing_context);

        maybe_parsed_number_t maybe_number = parse_number(parsing_context);

        if (maybe_number.ok) {
            maybe_instruction_line.instruction.operand = maybe_number.number;
            parsing_context->currentIndex += strlen(maybe_number.literal_form);
        } else {
            maybe_instruction_line.matched = false;
            goto shoud_return;
        }

        // @todo João, skip e atualização do index do parsing context, ainda precisamos parsear o operand
        // @todo João, avaliar as regras semânticas entram em ação aqui, algumas instruções tem comandos
        // parâmetro opcional e parâmetro padrão diferente de 0
    }
maybe_instruction_line.matched = true;
shoud_return:
    return maybe_instruction_line;
}


void test02()
{
    //static inst_t instructions[STACK_MAX_SIZE] = {0};
    //size_t instructions_size = 0;

    // Primeiro exemplo a ser parseado
    const char * text = "  .teste PUSH 25\n PUSH   50\nPLUS\n";

    parsing_context_t parsing_context = {
        .source = text,
        .currentIndex = 0,
        .source_length = strlen(text),
    };

    
    skip_whitespace(&parsing_context);
    
    assert(parsing_context.currentIndex == 2 && "Deveria estar no index dois");

    maybe_instruction_line_t maybe_instruction_line = parse_instruction_line(&parsing_context);

    assert(parsing_context.currentIndex == 16 && "Deveria estar no index dezesseis");

    if (maybe_instruction_line.matched) {
        printf(
            "parsed symbol: %s, type(opcode): %d, operand: %d",
            maybe_instruction_line.label,
            maybe_instruction_line.instruction.type,
            maybe_instruction_line.instruction.operand
        );
    }
}

int main()
{
    test01();
    test02();

    return EXIT_SUCCESS;
}
