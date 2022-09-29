#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "./vm-definitions.h"
#include "./compiler-requirements.h"

typedef struct parsing_context {
    const char *source;
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

void skip_whitespace(parsing_context_t *parsing_context)
{
    const char *source = parsing_context->source;
    char value; 

    while ((value = *source) && is_whitespace(value))
    {
        source++;
    }

    parsing_context->currentIndex += source - parsing_context->source;
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
    const char *source = parsing_context->source;
    char current_value;

    while ((current_value = *source) && (
        (current_value >= 'a' && 'z' >= current_value) ||
        (current_value >= 'A' && 'Z' >= current_value)
    ))
    {
        source++;
    }

    size_t symbol_size = source - parsing_context->source;

    if (current_value == '\0' || is_whitespace(current_value) ) {
        char * symbol = malloc(sizeof(char) * symbol_size + 1);
        memcpy(symbol, parsing_context->source, symbol_size);
        symbol[symbol_size] = '\0';
        maybe.ok = true;
        maybe.symbol = symbol;
    }

    return maybe;
}

/**
 * @brief Parseia um linha lógica. A linha é formada por uma etiqueta/marcação,
 * opcional, seguida por um nome de instrução
 * 
 * @param parsing_context 
 */
const maybe_instruction_line_t *parse_instruction_line(parsing_context_t *parsing_context)
{
    maybe_instruction_line_t maybe_instruction_line = {0};
    const char *source = parsing_context->source;
    char current_value = *source;
    if (current_value == '.') {
        parsing_context_t new_parsing_context = *parsing_context;
        new_parsing_context.currentIndex++;
        maybe_parsed_t maybe_parsed = parse_symbol(&new_parsing_context);

        if (maybe_parsed.ok) {
            maybe_instruction_line.label = maybe_parsed.symbol;
        } else {
            maybe_instruction_line.matched = false;
            goto shoud_return;
        }
        // @todo João, pode ser um label
    // } else if (current_value > '0' && current_value < ('9'+1)) { //  @todo João, fazer esse logo em seguida
        // @todo João, pode ser um número
    } else {
        // @todo João, pode ser um nome de instrução
    }

shoud_return:
    return &maybe_instruction_line;
}


void test02()
{
    static inst_t instructions[STACK_MAX_SIZE] = {0};
    size_t instructions_size = 0;

    // Primeiro exemplo a ser parseado
    const char * text = "  PUSH 25\n PUSH   50\nPLUS\n";

    parsing_context_t parsing_context = {
        .source = text,
        .currentIndex = 0,

    };

    
    skip_whitespace(&parsing_context);
    
    parse_instruction_line(&parsing_context);

    assert(parsing_context.currentIndex == 2 && "Deveria estar no index dois");

}

int main()
{
    test01();
    test02();

    return EXIT_SUCCESS;
}
