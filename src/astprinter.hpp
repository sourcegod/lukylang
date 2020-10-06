#ifndef ASTPRINTER_H
#define ASTPRINTER_H

#include "expr.hpp"
#include <string>
#include <vector>

class AstPrinter : public ExprVisitor {
public:
    std::string m_result;

    std::string print(Expr* expr);
    TObject visitBinaryExpr(BinaryExpr& expr);
    TObject visitGroupingExpr(GroupingExpr& expr);
    TObject visitLiteralExpr(LiteralExpr& expr); 
    TObject visitUnaryExpr(UnaryExpr& expr);
    std::string parenthesize(std::string name, std::vector<Expr*> exprs);
};

#endif // ASTPRINTER_H
