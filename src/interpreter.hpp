#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "expr.hpp"
#include "stmt.hpp"
#include "environment.hpp"
#include <string>
#include <vector>

class Interpreter : public ExprVisitor,  public StmtVisitor {
public:
    Interpreter();

    void interpret(std::vector<std::unique_ptr<Stmt>>&&);

    void visitExpressionStmt(ExpressionStmt&);
    void visitPrintStmt(PrintStmt&);
    void visitVarStmt(VarStmt& stmt);
    
    TObject visitAssignExpr(AssignExpr& expr);
    TObject visitBinaryExpr(BinaryExpr& expr);
    TObject visitGroupingExpr(GroupingExpr& expr);
    TObject visitLiteralExpr(LiteralExpr& expr); 
    TObject visitUnaryExpr(UnaryExpr& expr);
    TObject visitVariableExpr(VariableExpr& expr);

private:
    Environment environment;
    std::string m_result;
    TObject evaluate(PExpr& expr);
    void execute(PStmt stmt);
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
