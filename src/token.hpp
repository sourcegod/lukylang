#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

// namespace luky {
    enum class TokenType {
        // Single-character tokens.
        LEFT_PAREN, RIGHT_PAREN,
        LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, 
        MINUS, PLUS, SEMICOLON, 
        SLASH, STAR,

        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Reserved Keywords.
        AND, CLASS,
        ELSE, FALSE,
        FUN, FOR,
        IF, NIL,
        OR, PRINT,
        RETURN, SUPER,
        THIS, TRUE,
        VAR, WHILE,

        END_OF_FILE
    };

    class Token {
      public:
        Token() {}
        Token(TokenType _type, const std::string& _lexeme,
              const std::string& _literal, const int _line, const int _col);
        TokenType type;
        std::string lexeme;
        std::string literal;
        int line;
        int col;
        std::string toString() const;
        std::string stringType() const;
    };

// }

#endif // TOKEN_HPP
