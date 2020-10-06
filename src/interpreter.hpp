#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "expr.hpp"
#include <string>
#include <vector>

class Interpreter : public ExprVisitor {
public:
    Interpreter();
    std::string print(Expr* expr);
    void interpret(PExpr& expression);
    TObject visitBinaryExpr(BinaryExpr& expr);
    TObject visitGroupingExpr(GroupingExpr& expr);
    TObject visitLiteralExpr(LiteralExpr& expr); 
    TObject visitUnaryExpr(UnaryExpr& expr);
private: 
    std::string m_result;

    std::string stringify(TObject);

    std::string parenthesize(std::string name, std::vector<Expr*> exprs);
    TObject evaluate(PExpr& expr);

};

#endif // INTERPRETER_HPP
