#ifndef PARSER_HPP
#define PARSER_HPP

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include <memory>
#include <stdexcept>
#include <vector>
#include "lukobject.hpp"

// forward declarations
class LukError;
class LukObject;
class Stmt;

using PExpr = std::unique_ptr<Expr>;
using PStmt = std::unique_ptr<Stmt>;
using PObject = std::unique_ptr<LukObject>;

class ParseError : public std::runtime_error {
  public:
    ParseError(const std::string& msg, Token& token);
    Token m_token;
};

class Parser {
public:
    Parser(const std::vector<Token>& _tokens, LukError& lukErr);
    std::vector<PStmt> parse();
    ParseError error(Token& token, const std::string& message);

private:
    size_t current;
    std::vector<Token> tokens;
    LukError& lukErr;
    const std::string errTitle = "ParseError: ";

    PStmt statement();
    PStmt expressionStatement();
    PStmt ifStatement();
    PStmt printStatement();
    std::vector<PStmt> block();
    
    PExpr expression();
    PExpr assignment();
    PExpr logicOr();
    PExpr logicAnd();
    
    PStmt declaration();
    PStmt varDeclaration();
    
    PExpr equality();
    PExpr comparison();
    PExpr addition();
    PExpr multiplication();
    PExpr unary();
    PExpr primary();

    bool match(const std::vector<TokenType>& types);
    Token previous();
    Token advance();
    Token& peek();
    bool isAtEnd();
    bool check(TokenType type);
    Token consume(TokenType type, std::string message);
    void synchronize();

};

#endif // PARSER_HPP
