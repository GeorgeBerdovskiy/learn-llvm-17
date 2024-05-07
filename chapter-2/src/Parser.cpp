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

// We will use "panic mode" to recover from syntax errors
// In panic mode, tokens are deleted from the token stream
// until one is found that the parser can use to continue
// its work. Most programming languages have symbols that
// denote an end (ex - ; or }). Such tokens are good candidates
// to look for

_error:
    while(!Tok.is(Token::eoi))
        advance();
    return nullptr;
};

Expr *Parser::parseExpr() {
    Expr *Left = parseTerm();
    // The repeated group
    while (Tok.isOneOf(Token::plus, Token::minus)) {
        BinaryOp::Operator Op = 
            Tok.is(Token::plus) ? BinaryOp::Plus : BinaryOp::Minus;
        advance();
        Expr *Right = parseTerm();
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;
}

Expr *Parser::parseTerm() {
    Expr *Left = parseFactor();
    while (Tok.isOneOf(Token::star, Token::slash)) {
        BinaryOp::Operator Op =
            Tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
        advance();
        Expr *Right = parseFactor();
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;
}

Expr *Parser::parseFactor() {
    Expr *Res = nullptr;
    switch (Tok.getKind()) {
    case Token::number:
        Res = new Factor(Factor::Number, Tok.getText());
        advance();
        break;
    case Token::ident:
        Res = new Factor(Factor::Ident, Tok.getText());
        advance();
        break;
    case Token::l_paren:
        advance();
        Res = parseExpr();
        if (!consume(Token::r_paren)) break;
    default:
        if (!Res) error();
        while (!Tok.isOneOf(Token::r_paren, Token::star,
                            Token::plus, Token::minus,
                            Token::slash, Token::eoi))
            advance();
    }
    return Res;
}

// Once you'e memorized the patterns used, it's almost tedious work
// to code the parser based on the grammar rules. This type of parser
// is called a RECURSIVE DESCENT PARSER

// Note that A RECURSIVE DESCENT PARSER CAN'T BE CONSTRUCTED FROM
// EVERY GRAMMAR!

// This kind of parser is called LL(1) because it parses the input
// from Left to right, performing Leftmost deirvation of the sentence
// with 1 token lookahead.

// More generally, there are LL(k) parsers. An LL parser is called
// an LL(k) parser if it uses k tokens of lookahead when parsing
// a sentence. 

// It is easiest to build recursive descent parsers for LL(1) grammars,
// but it's also possible for LL(k) grammars. In fact, it's guaranteed
// to terminate for them! While other kinds of grammars can be checked
// using a recursive descent parser, it's more difficult to implement
// and most importantly, it's NOT guaranteed to terminate.

// Read more here - https://en.wikipedia.org/wiki/LL_parser

