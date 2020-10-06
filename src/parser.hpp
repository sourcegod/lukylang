#ifndef PARSER_HPP
#define PARSER_HPP

#include "expr.hpp"
#include "token.hpp"
#include <memory>
#include <stdexcept>
#include <vector>
#include "lukobject.hpp"

using PExpr = std::unique_ptr<Expr>;
// forward declarations
class LukError;
class LukObject;

class ParseError : public std::runtime_error {
  public:
    ParseError(const std::string& msg, Token& token);
    Token m_token;
};

class Parser {
public:
    Parser(const std::vector<Token>& _tokens, LukError& lukErr);
    size_t current;
    PExpr expression();
    PExpr equality();
    PExpr comparison();
    PExpr addition();
    PExpr multiplication();
    PExpr unary();
    PExpr primary();
    PExpr parse();
    ParseError error(Token& token, const std::string& message);

private:
    std::vector<Token> tokens;
    LukError& lukErr;

    bool match(const std::vector<TokenType>& types);
    Token previous();
    Token advance();
    Token& peek();
    bool isAtEnd();
    bool check(TokenType type);
    Token consume(TokenType type, std::string message);

};

#endif // PARSER_HPP
