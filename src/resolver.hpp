#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <memory> // smart pointers

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"
#include "lukobject.hpp"
#include "lukerror.hpp"
#include "logger.hpp"

class Interpreter;
class Token;
class LukObject;
class LukError;
class CTracer;

class Resolver : public ExprVisitor,  public StmtVisitor {
public:
  explicit Resolver(Interpreter& interp, LukError& lukErr);

  ~Resolver() {
    logMsg("\n~Resolver destructor");
  }
  
  void resolve(std::vector<std::shared_ptr<Stmt>>& statements);
    
    // expressions
    ObjPtr visitAssignExpr(AssignExpr& expr) override;
    ObjPtr visitBinaryExpr(BinaryExpr& expr) override;
    ObjPtr visitCallExpr(CallExpr& expr) override;
    ObjPtr visitFunctionExpr(FunctionExpr& expr);
    ObjPtr visitGetExpr(GetExpr& expr) override;
    ObjPtr visitGroupingExpr(GroupingExpr& expr) override;
    ObjPtr visitLiteralExpr(LiteralExpr& expr) override; 
    ObjPtr visitLogicalExpr(LogicalExpr& expr) override;
    ObjPtr visitSetExpr(SetExpr& expr) override;
    ObjPtr visitSuperExpr(SuperExpr& expr) override;
    ObjPtr visitThisExpr(ThisExpr& expr) override;
    ObjPtr visitUnaryExpr(UnaryExpr& expr);
    ObjPtr visitVariableExpr(VariableExpr& expr) override;
    
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
      None, Class, Subclass
    };
    
    ClassType  currentClass = ClassType::None;
    const std::string errTitle = "ResolverError: ";
  Interpreter& m_interp;
  LukError& m_lukErr;
  std::vector< std::unordered_map<std::string, bool> > m_scopes;
  FunctionType m_curFunction = FunctionType::None;

  // resolve expression
  void resolve(ExprPtr expr);
  void resolveLocal(Expr* expr, TokPtr& name);
  
  // resolve statements
  void resolve(StmtPtr& stmt);
  void resolveFunction(FunctionExpr& func, FunctionType ft);
  

  void beginScope();
  void endScope();
  void declare(TokPtr& name);
  void define(TokPtr& name);

};

#endif // RESOLVER_HPP
