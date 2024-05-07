#ifndef SEMA_H
#define SEMA_H

#include "AST.h"
#include "Lexer.hpp"

// The semantic analyzer walks the AST and checks
// various semantic rules of the language. For example,
// a variable must be declared before use or types of
// variables must be compatible in an expression.

// In this case, the semantic analyzer must check that
// each used variable is declareed because that is
// what the language requires.

class Sema {
public:
    bool semantic(AST *Tree);
};

#endif