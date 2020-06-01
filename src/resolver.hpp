#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory> // smart pointers

#include "expr.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include "token.hpp"
#include "lukobject.hpp"
#include "lukerror.hpp"
#include "logger.hpp"

class Interpreter;
class Token;
class LukObject;
class Environment;
class LukError;
class CTracer;

class Resolver : public ExprVisitor,  public StmtVisitor {
public:
  explicit Resolver(Interpreter& interp, LukError& lukErr);
  void resolve(std::vector<std::unique_ptr<Stmt>>& statements);
    
    // expressions
    TObject visitAssignExpr(AssignExpr& expr) override;
    TObject visitBinaryExpr(BinaryExpr& expr) override;
    TObject visitCallExpr(CallExpr& expr) override;
    TObject visitGroupingExpr(GroupingExpr& expr) override;
    TObject visitLiteralExpr(LiteralExpr& expr) override; 
    TObject visitLogicalExpr(LogicalExpr& expr) override;
    TObject visitUnaryExpr(UnaryExpr& expr);
    TObject visitVariableExpr(VariableExpr& expr) override;
    
    // statements
    void visitBlockStmt(BlockStmt& stmt) override;
    void visitBreakStmt(BreakStmt& stmt) override;
    void visitExpressionStmt(ExpressionStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    void visitFunctionStmt(FunctionStmt* stmt) override;
    void visitPrintStmt(PrintStmt& stmt) override;
    void visitReturnStmt(ReturnStmt& stmt) override;
    void visitVarStmt(VarStmt& stmt) override;
    void visitWhileStmt(WhileStmt& stmt) override;

private:
    const std::string errTitle = "ResolverError: ";
  Interpreter& m_interp;
  LukError& m_lukErr;
  std::vector< std::unordered_map<std::string, bool> > m_scopes;
  
  // resolve expression
  void resolve(PExpr& expr);
  void resolveLocal(Expr* expr, Token name);
  
  // resolve statements
  void resolve(PStmt& stmt);
  void resolveFunction(FunctionStmt* func);
  

  void beginScope();
  void endScope();
  void declare(Token name);
  void define(Token& name);

};

#endif // RESOLVER_HPP
