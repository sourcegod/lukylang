#include "token.hpp"

// using namespace luky;

Token::Token(const TokenType _type, const std::string& _lexeme,
             const std::string& _literal, const int _line, const int _col)
    : type(_type), lexeme(_lexeme),
    literal(_literal), line(_line), 
    col(_col) {}

std::string Token::toString() const {
    // for string and number literals, use actual value
    if (type == TokenType::STRING || 
        type == TokenType::NUMBER) {
        return literal;
    }

    return lexeme;
}

std::string Token::stringType() const {
    // returns token type in string
    switch(type) {
        case TokenType::IDENTIFIER: return "Ident";
        case TokenType::NUMBER: return "Number";
        case TokenType::STRING: return "String";
        default: return "";
    }

}
