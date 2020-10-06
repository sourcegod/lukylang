#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "expr.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include <string>
#include <vector>
using PObject = std::unique_ptr<LukObject>;
class Interpreter : public ExprVisitor,  public StmtVisitor {
public:
    PEnvironment m_globals;

    Interpreter();
    ~Interpreter() { }

    void interpret(std::vector<std::unique_ptr<Stmt>>&&);
    void printResult();
    
    void visitBlockStmt(BlockStmt& stmt);
    void visitBreakStmt(BreakStmt& stmt);
    void visitExpressionStmt(ExpressionStmt&);
    void visitFunctionStmt(FunctionStmt* stmt);
    void visitIfStmt(IfStmt& stmt);
    void visitPrintStmt(PrintStmt&);
    void visitVarStmt(VarStmt& stmt);
    void visitWhileStmt(WhileStmt& stmt);
    
    TObject visitAssignExpr(AssignExpr& expr);
    TObject visitBinaryExpr(BinaryExpr& expr);
    TObject visitCallExpr(CallExpr& expr);
    TObject visitGroupingExpr(GroupingExpr& expr);
    TObject visitLogicalExpr(LogicalExpr& expr);
    TObject visitLiteralExpr(LiteralExpr& expr); 
    TObject visitUnaryExpr(UnaryExpr& expr);
    TObject visitVariableExpr(VariableExpr& expr);

    TObject evaluate(PExpr& expr);
    void execute(PStmt stmt);
    void executeBlock(std::vector<PStmt>& statements, PEnvironment env);

private:
    PEnvironment m_environment;
    TObject m_result;
    const std::string errTitle = "InterpretError: ";
    bool isTruthy(TObject& obj);
    bool isEqual(TObject& a, TObject& b);
    void checkNumberOperand(Token& op, TObject& operand);
    void checkNumberOperands(Token& op, TObject& left, TObject& right);

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
