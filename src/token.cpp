#include "token.hpp"
using namespace luky;

int Token::next_id =0;
Token::Token() : id(++next_id) {
    logMsg("\nToken constructor, id: ", id, ", lexeme: None");
}

Token::Token(const TokenType _type, const std::string& _lexeme,
        const std::string& _literal, const int _line, const int _col)
        : id(++next_id), type(_type), 
        lexeme(_lexeme), literal(_literal), 
        line(_line), col(_col) {

    logMsg("\nToken constructor, id: ", id, ", lexeme: ", lexeme);
}

std::string Token::toString() const {
    // for string and number literals, use actual value
    if (type == TokenType::STRING || 
        type == TokenType::NUMBER || 
        type == TokenType::INT || 
        type == TokenType::DOUBLE) { 
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
        case TokenType::INT: return "Int";
        case TokenType::DOUBLE: return "Double";
        default: return "";
    }

}
