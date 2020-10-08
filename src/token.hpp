#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "common.hpp"
#include "logger.hpp"
#include <string>
#include <memory> // for smart pointers

// using TokPtr = std::shared_ptr<Token>;
enum class TokenType {
    // Single-character tokens.
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, 
    MINUS, PLUS, SEMICOLON, 
    SLASH, STAR,
    QUESTION, COLON, 

    // One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESSER, LESSER_EQUAL,
    PLUS_EQUAL, MINUS_EQUAL, 
    PLUS_PLUS, MINUS_MINUS,
    STAR_EQUAL, SLASH_EQUAL, 
    MOD, MOD_EQUAL,
    EXP, EXP_EQUAL,
    EQUAL_ARROW,

    // bitwise operators
    BIT_AND, BIT_AND_EQUAL, BIT_NOT,
    BIT_OR, BIT_OR_EQUAL, 
    BIT_XOR, BIT_XOR_EQUAL,
    BIT_LEFT, BIT_LEFT_EQUAL,
    BIT_RIGHT, BIT_RIGHT_EQUAL,

    // Literals.
    IDENTIFIER, STRING, NUMBER, 
    INT, DOUBLE,

    // Reserved Keywords.
    AND, BREAK, 
    CLASS, CONTINUE,
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
protected:
    static int next_id;
public:
    int id;
    TokenType type;
    std::string lexeme;
    std::string literal;
    int line;
    int col;
    
    // constructors
    Token(); 
    Token(TokenType _type, const std::string& _lexeme,
          const std::string& _literal, const int _line, const int _col);
    
    // destructors
    ~Token() {
        logMsg("\n~Token destructor, id: ", id, ", lexeme: ", lexeme);
    }
    
    std::string toString() const;
    std::string stringType() const;
    
    friend inline std::ostream& operator<<(std::ostream& ost, Token& tok);
    friend inline std::ostream& operator<<(std::ostream& ost, TokPtr& tokP);
};

// friend function declaration, because ostream accept only one argument
inline std::ostream& operator<<(std::ostream& ost, Token& tok) { return ost << tok.lexeme; } 
inline std::ostream& operator<<(std::ostream& ost, TokPtr& tokP) { return ost << tokP->lexeme; } 


#endif // TOKEN_HPP
