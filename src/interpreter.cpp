#include "interpreter.hpp"
#include "runtimeerror.hpp"
#include "jump.hpp"
#include "lukcallable.hpp"
#include "clock_func.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <typeinfo> // type name
#include <sstream> // for stringstream

Interpreter::Interpreter() {
    m_globals = std::make_shared<Environment>();
    m_environment = m_globals;
    // std::shared_ptr<LukCallable>  
    // auto func = std::make_shared<ClockFunc>();
    auto func = std::make_shared<ClockFunc>();
    m_globals->define("clock", LukObject(func));

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
    // std::cerr << "printResult avant \n";
    std::cout << stringify(m_result) << "\n";
    // std::cerr << "voici m_result.p_string: " << m_result.p_string << std::endl;
    // std::cerr << stringify(m_result) << "\n";
    // reinitialize m_result to nil
    // std::cerr << "voici m_result.p_string: " << m_result.p_string << std::endl;
    m_result = TObject();
    // std::cerr << "printResult à la fin,  m_result.p_string: " << m_result.p_string << std::endl;
    
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

void Interpreter::visitBreakStmt(BreakStmt& stmt) {
    std::string msg;
    if (stmt.keyword.lexeme == "break") {
        msg = "Error: 'break' must with while loop";
        throw Jump(stmt.keyword, msg);
    } else if (stmt.keyword.lexeme == "continue") {
        msg = "Error: 'continue' must with while loop";
        throw Jump(stmt.keyword, msg);
    }
         
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
    // std::cerr << "visitPrintStmt: avant value\n";
    TObject value = evaluate(stmt.expression);
    // std::cerr << "après value : " << value.p_string << std::endl;
    std::cout << stringify(value) << std::endl;
    m_result = TObject();
    // std::cerr << "visitPrintStmt: à la fin: \n";

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
        try {
            stmt.body->accept(*this);
            val  = evaluate(stmt.condition);
        } catch(Jump jmp) {
            if (jmp.keyword.lexeme == "break") break;
            if (jmp.keyword.lexeme == "continue") continue;
        }

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
                // return (std::string)left + (std::string)right;
                // Addingeach string to ostringstream
                return left + right;
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
TObject Interpreter::visitCallExpr(CallExpr& expr) {
    auto callee = evaluate(expr.callee);
    if (! callee.isCallable()) {
       throw RuntimeError(expr.paren, "Can only call function and class.");
    }
    
    std::vector<TObject> args;
    for (auto& arg: expr.args) {
        args.push_back(evaluate(arg));
    }
     
    auto func = callee.getCallable();
    if (args.size() != func->arity()) {
        std::ostringstream msg;
        msg << "Expected " << func->arity() 
            << " arguments but got " 
            << args.size() << ".";
        throw RuntimeError(expr.paren, msg.str());
    }

    return func->call(*this, args);

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
    auto obj = expr.value;
    // std::cout << "voici obj.p_string: " << obj.p_string << std::endl;
    // return expr.value;
    return obj;
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
    if (obj.isBool()) return obj.getBool();
    if (obj.isNumber() && obj.getNumber() == 0) return false;
    if (obj.isString() && obj.getString() == "") return false;
    
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

    // return obj.value();

    return *obj.getPtrString();
}


