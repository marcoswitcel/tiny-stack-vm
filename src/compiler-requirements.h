#ifndef COMPILER_REQUIREMENTS_H
#define COMPILER_REQUIREMENTS_H

#include <assert.h>

#include "./vm-definitions.h"

/**
 * @note Garante que a flag `-fshort-enums` tenha sido passada ao compilador,
 * isso é importante por hora pois os enums precisam ter um byte para o código
 * funcionar.
 */
static_assert(sizeof(instructions_t) == 1, "Enums de 1 byte com -fshort-enums não habilitado");

#endif // COMPILER_REQUIREMENTS_H
