#include "Lexer.hpp"

// These are some helper functions that will
// help us classify characters
namespace charinfo {
    // The `LLVM_READNONE` part is basically
    // just making it a const. function but "following
    // LLVM conventions"
    // (https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Variable-Attributes.html)
    LLVM_READNONE inline bool isWhitespace(char c) {
        return c == ' '
            || c == '\t' 
            || c == '\f'
            || c == '\v'
            || c == '\r'
            || c == '\n';
    }

    LLVM_READNONE inline bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    LLVM_READNONE inline bool isLetter(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z');
    }

    // We're not using the standard header lib.
    // functions for several reasons.
    // - Change behavior based on locale (ex -
    //   German umlauts)
    // - Since functions have `int` as param type,
    //   conversion from char type is required
    // Result of this conversion depends on
    // whether char is treated as a signed or
    // unsigned type, causing portability problems
}

void Lexer::next(Token &token) {
    // While there is still more to read
    // from the buffer and the current character
    // is whitespace of some kind, incr. the buffer
    // to move to the next token
    while (*BufferPtr &&
           charinfo::isWhitespace(*BufferPtr)) {
        // TODO: See if this _really_ needs to be
        // preincremented over incremented
        ++BufferPtr;
    }

    if (!*BufferPtr) {
        // There are no more characters left to process,
        // so we set the token to "eot"
        token.Kind = Token::eoi;
        return;
    }

    // At this point, there is _at least one_ token left
    if (charinfo::isLetter(*BufferPtr)) {
        const char *end = BufferPtr + 1;

        // Keep reading until we hit a character that isn't a letter
        while (charinfo::isLetter(*end))
            ++end;
        
        llvm::StringRef Name(BufferPtr, end - BufferPtr);
        Token::TokenKind kind = 
            Name == "with" ? Token::KW_with : Token::ident;
        formToken(token, end, kind);
        return;
    } else if (charinfo::isDigit(*BufferPtr)) {
        const char *end = BufferPtr + 1;
        while (charinfo::isDigit(*end))
            ++end;
        formToken(token, end, Token::number);
        return;
    } else {
        switch (*BufferPtr) {
#define CASE(ch, tok) \
case ch: formToken(token, BufferPtr + 1, tok); break
CASE('+', Token::plus);
CASE('-', Token::minus);
CASE('*', Token::star);
CASE('/', Token::slash);
CASE('(', Token::Token::l_paren);
CASE(')', Token::Token::r_paren);
CASE(':', Token::Token::colon);
CASE(',', Token::Token::comma);
#undef CASE
        default:
            formToken(token, BufferPtr + 1, Token::unknown);
        }
        return;
    }
}

void Lexer::formToken(Token &Tok, const char *TokEnd, Token::TokenKind Kind) {
    // Populates the members of the `Token` instance and updates
    // the pointer to the next unprocessed character
    Tok.Kind = Kind;
    Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
}