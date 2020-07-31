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
  void resolve(std::vector<std::shared_ptr<Stmt>>& statements);
    
    // expressions
    TObject visitAssignExpr(AssignExpr& expr) override;
    TObject visitBinaryExpr(BinaryExpr& expr) override;
    TObject visitCallExpr(CallExpr& expr) override;
    TObject visitGetExpr(GetExpr& expr) override;
    TObject visitGroupingExpr(GroupingExpr& expr) override;
    TObject visitLiteralExpr(LiteralExpr& expr) override; 
    TObject visitLogicalExpr(LogicalExpr& expr) override;
    TObject visitSetExpr(SetExpr& expr) override;
    TObject visitThisExpr(ThisExpr& expr) override;
    TObject visitUnaryExpr(UnaryExpr& expr);
    TObject visitVariableExpr(VariableExpr& expr) override;
    
    // statements
    void visitBlockStmt(BlockStmt& stmt) override;
    void visitBreakStmt(BreakStmt& stmt) override;
    void visitClassStmt(ClassStmt& stmt) override;
    void visitExpressionStmt(ExpressionStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    void visitFunctionStmt(FunctionStmt& stmt) override;
    void visitPrintStmt(PrintStmt& stmt) override;
    void visitReturnStmt(ReturnStmt& stmt) override;
    void visitVarStmt(VarStmt& stmt) override;
    void visitWhileStmt(WhileStmt& stmt) override;

private:
    // Note: using enum class as scoped enumeration
    // instead standard enumeration to avoid conflict between two enumerations.
    enum class FunctionType {
      None, Function, Initializer, Method
    };
   
    enum class ClassType {
      None, Class
    };
    
    ClassType  currentClass = ClassType::None;
    const std::string errTitle = "ResolverError: ";
  Interpreter& m_interp;
  LukError& m_lukErr;
  std::vector< std::unordered_map<std::string, bool> > m_scopes;
  FunctionType m_curFunction = FunctionType::None;

  // resolve expression
  void resolve(PExpr expr);
  void resolveLocal(Expr* expr, Token name);
  
  // resolve statements
  void resolve(PStmt& stmt);
  void resolveFunction(FunctionStmt& func, FunctionType ft);
  

  void beginScope();
  void endScope();
  void declare(Token name);
  void define(Token& name);

};

#endif // RESOLVER_HPP
