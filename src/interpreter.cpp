#include "interpreter.hpp"
#include <iostream>
#include <string>
#include <vector>

Interpreter::Interpreter() {}

std::string Interpreter::print(Expr* expr) {
    expr->accept(*this);
    std::cout << m_result << std::endl;
    m_result = "";
    return m_result;
}

void Interpreter::interpret(PExpr& expression) {
    try {
        auto val = evaluate(expression);
        // std::cout << m_result << std::endl;
        std::cout << stringify(val) << std::endl;
    } catch (std::runtime_error err) {
        std::cerr << "Interpreter Error: " << err.what() << "\n";
    }

    return;
}

std::string Interpreter::parenthesize(std::string name, std::vector<Expr*> v_expr) {
    m_result += "(" + name;
    for (auto expr : v_expr) {
        m_result += " ";
        expr->accept(*this);
    }

    m_result += ")";

    return m_result;
}

TObject Interpreter::visitBinaryExpr(BinaryExpr& expr) { return TObject(); }
TObject Interpreter::visitGroupingExpr(GroupingExpr& expr) { return TObject(); }
TObject Interpreter::visitLiteralExpr(LiteralExpr& expr) { 
  return expr.value; 
}
TObject Interpreter::visitUnaryExpr(UnaryExpr& expr) { return TObject(); }

TObject Interpreter::evaluate(PExpr& expr) { 
    return expr->accept(*this);
}

std::string Interpreter::stringify(TObject obj) { 
    return obj.toString();
}


