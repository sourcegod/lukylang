#include "interpreter.hpp"
#include "runtimeerror.hpp"
#include "jump.hpp"
#include "lukcallable.hpp"
#include "clock_func.hpp"
#include "lukfunction.hpp"
#include "return.hpp"
#include "logger.hpp"
#include "lukclass.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo> // type name
#include <sstream> // for stringstream

Interpreter::Interpreter() {
    LogConf.headers = true;
    LogConf.level = log_DEBUG;
    CLog(log_WARN) << "log_WARN: Coucou les gens";
    
    logMsg("\n--- Starts Interpreter");
    m_globals = std::make_shared<Environment>();
    m_environment = m_globals;
    m_globals->m_name = "Globals, " + m_globals->m_name;
    // TRACE_ALL;
    TRACE_MSG("Env globals tracer: ");
    // logMsg("Env globals: ", m_globals->m_name);
    auto func = std::make_shared<ClockFunc>();
    m_globals->define("clock", LukObject(func));

}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>&& statements) {
  logMsg("\n--- Starts Loop Interpreter");
    if (statements.empty()) 
        std::cerr << "Interp: vector is empty.\n";
    try {
         for (auto& stmt : statements) {
            if (stmt) {
                // execute(std::move(stmt));
                execute((stmt));
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

  logMsg("\n--- End Interpreter");

    return;
}

void Interpreter::printResult() {
    // CLog(log_DEBUG) << "printResult avant \n";
    std::cout << stringify(m_result) << "\n";
    // reinitialize m_result to nil
    m_result = TObject();
    
}


TObject Interpreter::evaluate(PExpr& expr) { 
    return expr->accept(*this);
}

void Interpreter::execute(PStmt& stmt) {
    stmt->accept(*this);
}

void Interpreter::resolve(Expr& expr, int depth) {
  // Note: FIX: abstract class Expr cannot be in map
  // so, we store its uniq id in the map
  m_locals[expr.id()] = depth;
}

void Interpreter::executeBlock(std::vector<PStmt>& statements, PEnvironment env) {
    auto previous = m_environment;
    try {
        m_environment = env;
        for (auto& stmt: statements) {
            if (stmt)
                // not use move because for reuse of the block
                // stmt->accept(*this);
                execute(stmt);

        }

    // Note: must catch all exceptions, even the Return exception.
    } catch(...) {
        m_environment = previous;
        // throw up the exception
        throw;
    }
    // finally, whether no exception
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

void Interpreter::visitClassStmt(ClassStmt& stmt) {
    m_environment->define(stmt.m_name.lexeme, TObject());
    auto klass = std::make_shared<LukClass>(stmt.m_name.lexeme);
    m_environment->assign(stmt.m_name, klass);

}

void Interpreter::visitExpressionStmt(ExpressionStmt& stmt) {
    m_result = evaluate(stmt.expression);
}

void Interpreter::visitFunctionStmt(FunctionStmt* stmt) {
    auto func = std::make_shared<LukFunction>(stmt, m_environment);
    m_environment->define(stmt->name.lexeme, LukObject(func));
    
}

void Interpreter::visitIfStmt(IfStmt& stmt) {
    auto val  = evaluate(stmt.condition);
    if (isTruthy(val)) {
        // Note: no moving statement pointer, because
        // it will be necessary later, for function call
        // execute(std::move(stmt.thenBranch));
        // stmt.thenBranch->accept(*this);
        execute(stmt.thenBranch);
    } else if (stmt.elseBranch != nullptr) {
        // execute(std::move(stmt.elseBranch));
        // stmt.elseBranch->accept(*this);
        execute(stmt.elseBranch);
    }

}

void Interpreter::visitPrintStmt(PrintStmt& stmt) {
    TObject value = evaluate(stmt.expression);
    std::cout << stringify(value) << std::endl;
    m_result = TObject();

}

void Interpreter::visitReturnStmt(ReturnStmt& stmt) {
    TObject value;
    if (stmt.value != nullptr) { 
        value = evaluate(stmt.value);
    } else {
        std::cout << "okok\n";
        value = nullptr;;
    }

    throw Return(value);
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
            execute(stmt.body);
            val  = evaluate(stmt.condition);
        } catch(Jump jmp) {
            if (jmp.keyword.lexeme == "break") break;
            if (jmp.keyword.lexeme == "continue") continue;
        }

    }

}

TObject Interpreter::visitAssignExpr(AssignExpr& expr) {
    TObject value = evaluate(expr.value);
    // search the variable in locals map, if not, search in the globals map.
    auto elem = m_locals.find(expr.id());
    if (elem != m_locals.end()) {
      auto val = std::make_shared<TObject>(value);
      m_environment->assignAt(elem->second, expr.name, val);
    } else {
      m_globals->assign(expr.name, value);
    }
    
    return value;
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
    
    std::vector<TObject> v_args;
    for (auto& arg: expr.args) {
        v_args.push_back(evaluate(arg));
    }
    const auto& func = callee.getCallable();
    if (v_args.size() != func->arity()) {
        std::ostringstream msg;
        msg << "Expected " << func->arity() 
            << " arguments but got " 
            << v_args.size() << ".";
        throw RuntimeError(expr.paren, msg.str());
    }

    return func->call(*this, v_args);
}

TObject Interpreter::visitGetExpr(GetExpr& expr) {
  auto obj = evaluate(expr.m_object);
  if (obj.isInstance()) {
    auto obj_ptr = obj.getInstance()->get(expr.m_name);
    return obj_ptr.get();
  }

  throw RuntimeError(expr.m_name,
    "Only instances have properties.");
  
  return TObject();
}

TObject Interpreter::visitGroupingExpr(GroupingExpr& expr) {
    return evaluate(expr.expression);
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
    return lookUpVariable(expr.name, expr);
}

TObject Interpreter::lookUpVariable(Token& name, Expr& expr) {
  // TODO: must be factorized
  // searching the depth in locals map
  // whether not, get the variable in globals map
  auto elem = m_locals.find(expr.id());
  if (elem != m_locals.end()) {
    // elem->second is the depth
    return m_environment->getAt(elem->second, name.lexeme);
  }

  return m_globals->get(name);
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
    if (obj.isNil() || obj.isBool()) return obj.value();
    if (obj.isNumber()) {
        std::string text = obj.value(); 
        std::string end = ".000000";
        // extract decimal part if ending by .0
        if (endsWith(text, end)) 
            return text.substr(0, text.size() - end.size());
        return text;
    } 
    
    // return obj.value();
    // std::cerr << "End of stringify\n";
    return *obj.getPtrString();
}


