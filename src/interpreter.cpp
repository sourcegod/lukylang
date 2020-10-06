#include "interpreter.hpp"
#include "runtimeerror.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <typeinfo> // type name

std::vector<std::unique_ptr<Stmt>> v_ptr;
Interpreter::Interpreter() {}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>&& statements) {
    if (statements.empty()) 
        std::cout << "Interp: vector is empty.\n";
    try {
         for (auto& stmt : statements) {
            if (stmt) {
                execute(std::move(stmt));
                // v_ptr.emplace_back(std::move(stmt));
                // v_ptr.back()->accept(*this);
            }
        }
        
    // } catch (std::runtime_error err) {
    // Note: passing exception by reference to avoid copy
    } catch (RuntimeError& err) {
        std::cerr << "Interpreter Error: " << err.what() << "\n";
    }


    return;
}

TObject Interpreter::evaluate(PExpr& expr) { 
    return expr->accept(*this);
}

void Interpreter::execute(PStmt stmt) {
    stmt->accept(*this);
}

void Interpreter::visitExpressionStmt(ExpressionStmt& stmt) {
    // std::cout << "visitExpressionStmt\n";
    // std::string name = typeid(stmt.expression).name();
    // std::cout << "name: " << name << std::endl;
    // stmt.expression->accept(*this);
    evaluate(stmt.expression);
}

void Interpreter::visitPrintStmt(PrintStmt& stmt) {
    TObject value = evaluate(stmt.expression);
    std::cout << stringify(value) << std::endl;

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

    return TObject();
}

TObject Interpreter::visitGroupingExpr(GroupingExpr& expr) {
    return evaluate(expr.expression);
    // return TObject();
}


TObject Interpreter::visitLiteralExpr(LiteralExpr& expr) {
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

std::string Interpreter::stringify(TObject& obj) { 
    if (obj.isNumber()) {
        std::string text = obj.value(); 
        std::string end = ".000000";
        // extract decimal part if ending by .0
        if (endsWith(text, end)) 
            return text.substr(0, text.size() - end.size());
        return text;
    }

    return obj.value();
}


