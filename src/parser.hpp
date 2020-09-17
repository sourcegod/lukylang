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

using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;
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
    std::vector<StmtPtr> parse();
    ParseError error(TokPtr& tokP, const std::string& message);

private:
    size_t m_current;
    std::vector<TokPtr> m_tokens;
    LukError& lukErr;
    const std::string errTitle = "ParseError: ";

    StmtPtr statement();
    std::vector<StmtPtr> block();
    StmtPtr breakStatement();
    StmtPtr classDeclaration();
    StmtPtr declaration();
    StmtPtr expressionStatement();
    StmtPtr forStatement();
    FuncPtr function(const std::string& kind);
    StmtPtr ifStatement();
    StmtPtr printStatement();
    StmtPtr returnStatement();
    StmtPtr varDeclaration();
    StmtPtr whileStatement();
    
    ExprPtr expression();
    std::shared_ptr<FunctionExpr> functionBody(const std::string& kind);
    ExprPtr assignment();
    ExprPtr compoundAssignment(ExprPtr left, TokPtr op);
    ExprPtr conditional();
    ExprPtr logicOr();
    ExprPtr logicAnd();
    
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr addition();
    ExprPtr multiplication();
    ExprPtr unary();
    ExprPtr call();
    ExprPtr finishCall(ExprPtr callee);
    ExprPtr primary();

    TokPtr& consume(TokenType type, std::string message);
    bool match(const std::vector<TokenType>& types);
    bool matchNext(const std::vector<TokenType>& types);
    TokPtr& previous();
    TokPtr& advance();
    TokPtr& peek();
    bool isAtEnd();
    bool check(TokenType type);
    bool checkNext(TokenType type);
    void synchronize();

};

#endif // PARSER_HPP
