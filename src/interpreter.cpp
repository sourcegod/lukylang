#include "interpreter.hpp"
#include "runtimeerror.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <typeinfo> // type name

Interpreter::Interpreter() {
    m_environment = std::make_shared<Environment>();
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>&& statements) {
    if (statements.empty()) 
        std::cerr << "Interp: vector is empty.\n";
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
        std::cerr << errTitle << err.what() << "\n";
    }
    // m_result += "tata ";
    if (!m_result.isNil())
        printResult();


    return;
}

void Interpreter::printResult() {
    std::cout << stringify(m_result) << "\n";
    // reinitialize m_result to nil
    m_result = TObject();
    

}


TObject Interpreter::evaluate(PExpr& expr) { 
    // std::cerr << "evaluate\n";
    return expr->accept(*this);
}

void Interpreter::execute(PStmt stmt) {
    stmt->accept(*this);
}

void Interpreter::executeBlock(std::vector<PStmt>& statements, PEnvironment env) {
    auto previous = m_environment;
    
    try {
        m_environment = env;
        
        for (auto& stmt: statements) {
            if (stmt)
                // not use move because for reuse of the block
                stmt->accept(*this);

        }

            
    } catch(RuntimeError& err) {
        m_environment = previous;
    }
    // finally
    m_environment = previous;

}

void Interpreter::visitBlockStmt(BlockStmt& stmt) {
    executeBlock(stmt.statements, std::make_shared<Environment>(m_environment) );
}

void Interpreter::visitExpressionStmt(ExpressionStmt& stmt) {
    m_result = evaluate(stmt.expression);
}

void Interpreter::visitIfStmt(IfStmt& stmt) {
    auto val  = evaluate(stmt.condition);
    if (isTruthy(val)) {
        execute(std::move(stmt.thenBranch));
    } else if (stmt.elseBranch != nullptr) {
        execute(std::move(stmt.elseBranch));
    }

}

void Interpreter::visitPrintStmt(PrintStmt& stmt) {
    TObject value = evaluate(stmt.expression);
    std::cout << stringify(value) << std::endl;
    m_result = TObject();

}

void Interpreter::visitVarStmt(VarStmt& stmt) {
    TObject value;
    if (stmt.initializer != nullptr) {
        value = evaluate(stmt.initializer);
    }
    m_environment->define(stmt.name.lexeme, value);

}

void Interpreter::visitWhileStmt(WhileStmt& stmt) {
    auto val  = evaluate(stmt.condition);
    while (isTruthy(val)) {
        stmt.body->accept(*this);
        val  = evaluate(stmt.condition);
    }

}

TObject Interpreter::visitAssignExpr(AssignExpr& expr) {
    TObject value = evaluate(expr.value);
    m_environment->assign(expr.name, value);
    return value;
}

TObject Interpreter::visitBinaryExpr(BinaryExpr& expr) {
    // std::cerr << "visitBinaryExpr\n";
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
    // unrichable
    return TObject();
}

TObject Interpreter::visitGroupingExpr(GroupingExpr& expr) {
    return evaluate(expr.expression);
    // return TObject();
}

TObject Interpreter::visitLogicalExpr(LogicalExpr& expr) {
    TObject left = evaluate(expr.left);
    if (expr.op.type == TokenType::OR) {
        if (isTruthy(left)) return left;
    } else {
        if (!isTruthy(left)) return left;
    }
    
    return evaluate(expr.right);
}

TObject Interpreter::visitLiteralExpr(LiteralExpr& expr) {
    // std::cerr << "visitLiteralExpr\n";
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

TObject Interpreter::visitVariableExpr(VariableExpr& expr) {
    return m_environment->get(expr.name);
}


bool Interpreter::isTruthy(TObject& obj) {
    if (obj.isNil()) return false;
    if (obj.isBool()) return (bool)obj;
    if (obj.isNumber() && (double)obj == 0) return false;

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


