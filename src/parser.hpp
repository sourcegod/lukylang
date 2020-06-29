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

using PExpr = std::shared_ptr<Expr>;
using PStmt = std::shared_ptr<Stmt>;
using PObject = std::shared_ptr<LukObject>;

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& msg, Token& token);
    Token m_token;
};

class Parser {
public:
    Parser(const std::vector<Token>&& tokens, LukError& lukErr);
    std::vector<PStmt> parse();
    ParseError error(Token& token, const std::string& message);

private:
    size_t current;
    std::vector<Token> m_tokens;
    LukError& lukErr;
    const std::string errTitle = "ParseError: ";

    PStmt statement();
    std::vector<PStmt> block();
    PStmt breakStatement();
    PStmt classDeclaration();
    PStmt declaration();
    PStmt expressionStatement();
    PStmt forStatement();
    PStmt function(std::string kind);
    PStmt ifStatement();
    PStmt printStatement();
    PStmt returnStatement();
    PStmt varDeclaration();
    PStmt whileStatement();
    
    PExpr expression();
    PExpr assignment();
    PExpr logicOr();
    PExpr logicAnd();
    
   
    PExpr equality();
    PExpr comparison();
    PExpr addition();
    PExpr multiplication();
    PExpr unary();
    PExpr call();
    PExpr finishCall(PExpr callee);
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
