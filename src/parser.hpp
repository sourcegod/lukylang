#ifndef PARSER_HPP
#define PARSER_HPP

#include "expr.hpp"
#include "token.hpp"
#include <memory>
#include <stdexcept>
#include <vector>
#include "lukobject.hpp"

using PExpr = std::unique_ptr<Expr>;
// namespace luky {
// forward declarations
class LukError;
class LukObject;

class ParseError : public std::runtime_error {
  public:
    ParseError(std::string msg, Token token);
    Token token_;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens, LukError& lukErr);
    size_t current;
    PExpr expression();
    PExpr equality();
    PExpr comparison();
    PExpr term();
    PExpr factor();
    PExpr unary();
    PExpr primary();
    PExpr parse();
    ParseError error(Token token, std::string message);

private:
    bool match(const std::vector<TokenType>& types);
    Token previous();
    Token advance();
    Token peek();
    bool isAtEnd();
    bool check(TokenType type);
    Token consume(TokenType type, std::string message);
    LukError& lukErr;
    std::vector<Token> tokens_;
};
// }

#endif // PARSER_HPP
