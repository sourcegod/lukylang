#include "astprinter.hpp"
#include <iostream>
#include <string>
#include <vector>


std::string AstPrinter::print(Expr* expr) {
    expr->accept(*this);
    std::cout << m_result << std::endl;
    m_result = "";
    return m_result;
}

TObject AstPrinter::visitBinaryExpr(BinaryExpr& expr) {
    // method get allow to convert smart pointer to raw pointer
    TObject obj =  parenthesize(expr.op.lexeme, 
            {expr.left.get(), expr.right.get()});
    return obj;
}

TObject AstPrinter::visitGroupingExpr(GroupingExpr& expr) {
    TObject obj = parenthesize("group", 
            {expr.expression.get()});

    return obj;
}


TObject AstPrinter::visitLiteralExpr(LiteralExpr& expr) {
    m_result += expr.value.toString();

    return TObject(m_result);

}

TObject AstPrinter::visitUnaryExpr(UnaryExpr& expr) {
    TObject obj =  parenthesize(expr.op.lexeme, 
            {expr.right.get()});

    return obj;
}


std::string AstPrinter::parenthesize(std::string name, std::vector<Expr*> v_expr) {
    m_result += "(" + name;
    for (auto expr : v_expr) {
        m_result += " ";
        expr->accept(*this);
    }

    m_result += ")";

    return m_result;
}


