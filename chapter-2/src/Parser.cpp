#include "Parser.hpp"

AST *Parser::parse() {
    AST *Res = parseCalc();
    expect(Token::eoi);
    return Res;
}

AST *Parser::parseCalc() {
    Expr *E;

    // We use LLVM's optimized "small" vector
    // instead of the standard lib. vector here
    llvm::SmallVector<llvm::StringRef, 8> Vars;

    // First decision to be made is whether the
    // optional group must be parsed or not. The
    // group begins with the 'with' token, so
    // let's compare the token to this val.

    if (Tok.is(Token::KW_with))
        advance();
    
    // Next, we expect an ident.
    if (expect(Token::ident))
        goto _error;
    
    // If there _is_ an identifier, then we
    // save it in the `Vars` vector. Otherwise,
    // it is a syntax error, handled sperately
    Vars.push_back(Tok.getText());
    advance();

    // Next follows a repeating group that
    // parses more identifiers, separated by commas
    while (Tok.is(Token::comma)) {
        advance();
        if (expect(Token::ident))
            goto _error;
        Vars.push_back(Tok.getText());
        advance();
    }

    // Finally, the optional group requires a
    // colon at the end
    if (consume(Token::colon))
        goto _error;
    
    E = parseExpr();

    // The collected information is now used to create
    // the AST node for this rule
    if (Vars.empty()) return E;
    else return new WithDecl(Vars, E);
}