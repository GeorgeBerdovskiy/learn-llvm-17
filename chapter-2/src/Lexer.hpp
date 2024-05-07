#ifndef LEXER_H
#define LEXER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"

// The `llvm::MemoryBuffer` class provides read-only access
// to a block of memory, filled with the content of a file.
// On request, a trailing zero character ('\x00') is added
// to the end of the buffer. We use this feature to read
// through the buffer without checking the length of the
// buffer at each access.

// The `llvm::StringRef` class encapsulates a pointer to a C
// string and its length. Because the length is stored, the string
// need not be terminated with a zero character ('\x00') like
// normal C strings. This allows an instance of StringRef to point
// to the memory managed by `MemoryBuffer`

class Lexer;

class Token {
    // Reminder - A friend class can access private and protected
    // members of other classes in which it is declared as a friend.
    // That means `Lexer` can access private / protected methods in Token
    friend class Lexer;

public:
    // eoi stands for "end of input" and is returned when all chars. of
    // the input are processed. "unknown" is used in the event of an
    // error at the lexical level (example - `#` is no token of the lang.
    // and would therefore be mapped to `unknown`).
    enum TokenKind : unsigned short {
        eoi, unknown, ident, number, comma, colon, plus,
        minus, star, slash, l_paren, r_paren, KW_with
    };

private:
    TokenKind Kind;       // Kind of token (see above def.)
    llvm::StringRef Text; // Points to the start of the text of the token
                          // helpful for semantic processing (ex. for identifier,
                          // useful to know the name)
public:
    TokenKind getKind() const { return Kind; }
    llvm::StringRef getText() const { return Text; }

    bool is(TokenKind K) const { return Kind == K; }
    bool isOneOf(TokenKind K1, TokenKind K2) const {
        return is(K1) || is(K2);
    }

    // Variadic template (see https://en.cppreference.com/w/cpp/language/parameter_pack)
    template <typename... Ts>
    bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const {
        return is(K1) || isOneOf(K2, Ks...);
    }
};

class Lexer {
    const char *BufferStart;
    const char *BufferPtr;
public:
    Lexer(const llvm::StringRef &Buffer) {
        BufferStart = Buffer.begin();
        BufferPtr = BufferStart;
    }
    void next(Token &token);
private:
    void formToken(Token &Result, const char *TokEnd, Token::TokenKind Kind);
};

#endif