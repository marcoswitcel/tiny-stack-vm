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

int main()
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

    
    return EXIT_SUCCESS;
}