#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Lexer.hpp"

// The coding guidelines from LLVM forbid the use of the <iostream> library
// Thus, we use the header of the equivalent LLVM functionality
#include "llvm/Support/raw_ostream.h"

class Parser {
    Lexer &Lex; // Used to retrieve the next token from the input
    Token Tok; // Stores the next token (look-ahead)
    bool HasError; // Indicates whether an error was detected

    void error() {
        llvm::errs() << "Unexpected: " << Tok.getText()
                     << "\n";
        HasError = true;
    }

    // Retrieves the next token from the lexer (when we say "retrieve",
    // we don't mean it's returning it though)
    void advance() { Lex.next(Tok); }

    // Test whether the look-ahead has the expected kind and
    // emits an error message if not. Sometimes called `match`
    bool expect(Token::TokenKind Kind) {
        if (Tok.getKind() != Kind) {
            error();
            return true;
        }
        return false;
    }

    // Retrieves the next token if the look-ahead
    // has expected kind. If an error message is emitted,
    // the `HasError` flag is set to true.
    bool consume(Token::TokenKind Kind) {
        if (expect(Kind))
            return true;
        advance()
        return false;
    }

    // NOTE - There are no methods for `ident` and `number`
    // Those rules only return the token and are replaced
    // by the corresponding token

    // (Almost) parsing entrypoint
    AST *parseCalc();
    Expr *parseExpr();
    Expr *parseTerm();
    Expr *parseFactor();

public:
    // Initializes all members and retrieves the first
    // token from the lexer
    Parser(Lexer &Lex) : Lex(Lex), HasError(false) {
        advance();
    }

    bool hasError() { return HasError; }

    // Parsing entrypoint
    AST *parse();
}

#endif