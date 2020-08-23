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
    ParseError(const std::string& msg, TokPtr& tokP);

    TokPtr m_token;
};

class Parser {
public:
    Parser(const std::vector<TokPtr>&& tokens, LukError& lukErr);
    
    ~Parser() {
      logMsg("\n~Parser destructor");

    }
    std::vector<PStmt> parse();
    ParseError error(TokPtr& tokP, const std::string& message);

private:
    size_t current;
    std::vector<TokPtr> m_tokens;
    LukError& lukErr;
    const std::string errTitle = "ParseError: ";

    PStmt statement();
    std::vector<PStmt> block();
    PStmt breakStatement();
    PStmt classDeclaration();
    PStmt declaration();
    PStmt expressionStatement();
    PStmt forStatement();
    PFunc function(const std::string& kind);
    PStmt ifStatement();
    PStmt printStatement();
    PStmt returnStatement();
    PStmt varDeclaration();
    PStmt whileStatement();
    
    PExpr expression();
    std::shared_ptr<FunctionExpr> functionBody(const std::string& kind);
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
    TokPtr& previous();
    TokPtr& advance();
    TokPtr& peek();
    bool isAtEnd();
    bool check(TokenType type);
    bool checkNext(TokenType type);
    TokPtr& consume(TokenType type, std::string message);
    void synchronize();

};

#endif // PARSER_HPP
