#include "interpreter.hpp"
// #include "lukerror.hpp"
#include "runtimeerror.hpp"
#include <iostream>
#include <string>
#include <vector>

Interpreter::Interpreter() {}
    // : lukErr(_lukErr) {}

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
    // } catch (std::runtime_error err) {
    // Note: passing exception by reference to avoid copy
    } catch (RuntimeError& err) {
        std::cerr << "Interpreter Error: " << err.what() << "\n";
    }

    return;
}

TObject Interpreter::visitBinaryExpr(BinaryExpr& expr) {
    // method get allow to convert smart pointer to raw pointer
    TObject left = evaluate(expr.left);
    TObject right = evaluate(expr.right);
    switch(expr.op.type) {
        case TokenType::GREATER:
            checkNumberOperands(expr.op, left, right);
            return (double)left > (double)right;
        
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.op, left, right);
            return (double)left >= (double)right;

         case TokenType::LESS:
            checkNumberOperands(expr.op, left, right);
            return (double)left < (double)right;

         case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.op, left, right);
            return (double)left <= (double)right;
   
        case TokenType::BANG_EQUAL: return !isEqual(left, right);
        case TokenType::EQUAL_EQUAL: return isEqual(left, right);

        case TokenType::MINUS:
            checkNumberOperands(expr.op, left, right);
            return (double)left - (double)right;
        
        case TokenType::PLUS:
            if (left.isNumber() && right.isNumber())
                return (double)left + (double)right; 
            if (left.isString() && right.isString())
                return (std::string)left + (std::string)right; 
            throw RuntimeError(expr.op, 
                    "Operands must be two numbers or tow strings.");

        case TokenType::SLASH:
            checkNumberOperands(expr.op, left, right);
            return (double)left / (double)right;

        case TokenType::STAR:
            checkNumberOperands(expr.op, left, right);
            return (double)left * (double)right;
         default: break;
    }

    /*
    TObject obj =  parenthesize(expr.op.lexeme, 
            {expr.left.get(), expr.right.get()});
    */

    return TObject();
}

TObject Interpreter::visitGroupingExpr(GroupingExpr& expr) {
    return evaluate(expr.expression);
    // return TObject();
}


TObject Interpreter::visitLiteralExpr(LiteralExpr& expr) {
    // m_result += expr.value.toString();
    return expr.value;

}

TObject Interpreter::visitUnaryExpr(UnaryExpr& expr) {
    TObject right = evaluate(expr.right);
    switch(expr.op.type) {
        case TokenType::BANG:
            return !isTruthy(right);
        
        case TokenType::MINUS:
            checkNumberOperand(expr.op, right);
            return -(double)right;
        default: break;
    }


    return TObject();
}

bool Interpreter::isTruthy(TObject& obj) {
    if (obj.isNil()) return false;
    if (obj.isBool()) return (bool)obj;

    return true;
}

bool Interpreter::isEqual(TObject& a, TObject& b) {
    // nil is only equal to nil
    if (a.isNil() && b.isNil()) return true;
    if (a.isNil()) return false;

    return a == b;
}

void Interpreter::checkNumberOperand(Token& op, TObject& operand) {
    if (operand.isNumber()) return;
    // throw std::runtime_error("From throw: Operand must be number.");
    throw RuntimeError(op, "Operand must be number.");
}

void Interpreter::checkNumberOperands(Token& op, TObject& left, TObject& right) {
    if (left.isNumber() && right.isNumber()) return;
    throw RuntimeError(op, "Operands must be numbers.");
    // throw std::runtime_error("Yes, Operands must be numbers.");

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

TObject Interpreter::evaluate(PExpr& expr) { 
    return expr->accept(*this);
}

std::string Interpreter::stringify(TObject obj) { 
    if (obj.isNumber()) {
        std::string text = obj.toString(); 
        std::string end = ".000000";
        // extract decimal part if ending by .0
        if (endsWith(text, end)) 
            return text.substr(0, text.size() - end.size());
        return text;
    }

    return obj.toString();
}


