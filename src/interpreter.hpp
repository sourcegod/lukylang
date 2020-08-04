#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "expr.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Interpreter : public ExprVisitor,  public StmtVisitor {
public:
    PEnvironment m_globals;

    Interpreter();
    ~Interpreter() { 
      logMsg("\n~Interpreter destructor\n");
    }

    void interpret(std::vector<std::shared_ptr<Stmt>> statements);
    void printResult();
    void logState();
    void logTest();

    // statements    
    void visitBlockStmt(BlockStmt& stmt) override;
    void visitBreakStmt(BreakStmt& stmt) override;
    void visitClassStmt(ClassStmt& stmt) override;
    void visitExpressionStmt(ExpressionStmt&) override;
    void visitFunctionStmt(FunctionStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    void visitPrintStmt(PrintStmt&) override;
    void visitReturnStmt(ReturnStmt& stmt) override;
    void visitVarStmt(VarStmt& stmt) override;
    void visitWhileStmt(WhileStmt& stmt) override;
    
    // expressions
    TObject visitAssignExpr(AssignExpr& expr) override;
    TObject visitBinaryExpr(BinaryExpr& expr) override;
    TObject visitCallExpr(CallExpr& expr) override;
    TObject visitGetExpr(GetExpr& expr);
    TObject visitGroupingExpr(GroupingExpr& expr) override;
    TObject visitLogicalExpr(LogicalExpr& expr) override;
    TObject visitLiteralExpr(LiteralExpr& expr) override; 
    TObject visitSetExpr(SetExpr& expr);
    TObject visitSuperExpr(SuperExpr& expr);
    TObject visitThisExpr(ThisExpr& expr);
    TObject visitUnaryExpr(UnaryExpr& expr) override;
    TObject visitVariableExpr(VariableExpr& expr) override;

    TObject evaluate(PExpr expr);
    void execute(PStmt& stmt);
    void resolve(Expr& expr, int depth);
    void executeBlock(std::vector<PStmt>& statements, PEnvironment env);

private:
    PEnvironment m_environment;
    TObject m_result;
    const std::string errTitle = "InterpretError: ";
    std::unordered_map<unsigned, int> m_locals;

    bool isTruthy(TObject& obj);
    bool isEqual(TObject& a, TObject& b);
    void checkNumberOperand(Token& op, TObject& operand);
    void checkNumberOperands(Token& op, TObject& left, TObject& right);
    TObject lookUpVariable(Token& name, Expr& expr);

    // starts and ends for string
    inline bool startsWith(const std::string& str, const std::string& start) {
        if (start.size() > str.size()) return false;
        return str.compare(0, start.size(), start) == 0;
    }

    inline bool endsWith(const std::string& str, const std::string& end) {
        if (end.size() > str.size()) return false;
        return std::equal(end.rbegin(), end.rend(), str.rbegin());
    }

    
    std::string stringify(TObject& obj);

};

#endif // INTERPRETER_HPP
