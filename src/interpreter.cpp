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
    logMsg("\nIn Interpreter constructor");
    LogConf.headers = true;
    LogConf.level = log_DEBUG;
    CLog(log_WARN) << "log_WARN: Coucou les gens";
    
    logMsg("\n--- Starts Interpreter");
    m_globals = std::make_shared<Environment>();
    m_environment = m_globals;
    m_globals->m_name = "Globals, " + m_globals->m_name;
    // TRACE_ALL;
    TRACE_MSG("Env globals tracer: ");
    auto func = std::make_shared<ClockFunc>();
    m_globals->define("clock", LukObject(func));
    logMsg("\nExit out Interpreter constructor");

}

void Interpreter::interpret(std::vector<std::shared_ptr<Stmt>> statements) {
  logMsg("\nIn Interpret, starts loop");
    if (statements.empty()) 
        std::cerr << "Interp: vector is empty.\n";
    logState();
    try {
         for (auto& stmt : statements) {
            if (stmt) {
                execute((stmt));
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
    logState();

  logMsg("\nExit out  Interpret");

    return;
}

void Interpreter::printResult() {
    // CLog(log_DEBUG) << "printResult avant \n";
    std::cout << stringify(m_result) << "\n";
    // reinitialize m_result to nil
    m_result = TObject();
    
}


void Interpreter::logState() {
#ifdef DEBUG
  logMsg("\nEnvironment state");
 // Note: workaound to make an alias for a variable in c++ 
 // int a; 
 // int* const b = &a;
 // b is an alias or pointer to a
 // but not work for a map
  // auto* values = &m_globals->m_values;
  logMsg("Globals state");
  auto& values = m_globals->getValues();
  // Note: Pattern: looping over map
  if (values.empty()) {
      logMsg("m_globals env is empty");
  } else {
      // for (auto& iter: m_globals->m_values)  {
      for (auto& iter: values)  {
          logMsg(iter.first, ":", iter.second->toString());
      }
  }

  logMsg("\nLocals state");
  if (m_locals.empty()) {
      logMsg("m_locals env is empty");
  } else {
      for (auto& iter: m_locals)  {
          logMsg(iter.first, ":", iter.second);
      }
  
  }
#endif

}


TObject Interpreter::evaluate(PExpr expr) { 
    return expr->accept(*this);
}

void Interpreter::execute(PStmt& stmt) {
    logMsg("\nIn execute top level, *stmt: ", typeid(*stmt).name());
    stmt->accept(*this);
}

void Interpreter::resolve(Expr& expr, int depth) {
  logMsg("\nIn Resolve expr, Interpreter");
  // Note: FIX: abstract class Expr cannot be in map
  // so, we store its uniq id in the map
  m_locals[expr.id()] = depth;
}

void Interpreter::executeBlock(std::vector<PStmt>& statements, PEnvironment env) {
    logMsg("\nIn ExecuteBlock: ");
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
    
    logMsg("\nExit out  ExecuteBlock: ");
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
  logMsg("In visitClassStmt: name: ", stmt.m_name.lexeme);
  TObject superclass;
  std::shared_ptr<LukClass> supKlass = nullptr;
  if (stmt.m_superclass != nullptr) {
    // Note: changing evaluate(PExpr&) to evaluate(Pexpr) to passing VariableExpr object
    superclass = evaluate(stmt.m_superclass);
    logMsg("superclass: ", superclass);
    // TODO: It will better to test whether superclass is classable instead callable
    if (!superclass.isCallable()) { //  instanceof LoxClass)) {
      throw RuntimeError(stmt.m_superclass->name,
            "Superclass must be a class.");
    } else {
      supKlass = superclass.getDynCast<LukClass>();
    }

  }
  m_environment->define(stmt.m_name.lexeme, TObject());

  if (stmt.m_superclass != nullptr) {
    m_environment = std::make_shared<Environment>(m_environment);
    m_environment->define("super", superclass);

  }

  std::unordered_map<std::string, std::shared_ptr<LukObject>> methods;
  for (auto meth: stmt.m_methods) {
    auto func = std::make_shared<LukFunction>(meth, m_environment,
        meth->name.lexeme == "init");
    logMsg("func name: ", func->toString());
    auto obj_ptr = std::make_shared<LukObject>(func);
    logMsg("obj_ptr type: ", obj_ptr->getType());
    logMsg("ObjPtr callable: ", obj_ptr->getCallable()->toString());
    logMsg("Adding meth to methods map: ", meth->name.lexeme);
    methods[meth->name.lexeme] = obj_ptr;
  }

  auto klass = std::make_shared<LukClass>(stmt.m_name.lexeme, supKlass, methods);
  // auto klass = std::make_shared<LukClass>(stmt.m_name.lexeme, methods);
  if (stmt.m_superclass != nullptr) {
    // Note: moving m_enclosing from private to public in Environment object
    m_environment = m_environment->m_enclosing;
  }
  logMsg("Assign klass: ", stmt.m_name, " to m_environment");
  m_environment->assign(stmt.m_name, LukObject(klass));
logMsg("Exit out visitClassStmt\n");
}

void Interpreter::visitExpressionStmt(ExpressionStmt& stmt) {
    m_result = evaluate(stmt.expression);
}

void Interpreter::visitFunctionStmt(FunctionStmt& stmt) {
    auto stmtPtr = std::make_shared<FunctionStmt>(stmt);
    auto func = std::make_shared<LukFunction>(stmtPtr, m_environment, false);
    m_environment->define(stmt.name.lexeme, LukObject(func));
    logMsg("FunctionStmt use_count: ", stmtPtr.use_count());
    
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
    logMsg("\nIn visitprint: value: ", value, ", id: ", value.getId());
    std::cout << stringify(value) << std::endl;
    m_result = TObject();

}

void Interpreter::visitReturnStmt(ReturnStmt& stmt) {
    TObject value;
    if (stmt.value != nullptr) { 
        value = evaluate(stmt.value);
    } else {
        // value = nullptr;
    }

    throw Return(value);
}


void Interpreter::visitVarStmt(VarStmt& stmt) {
    TObject value;
    if (stmt.initializer != nullptr) {
        value = evaluate(stmt.initializer);
    }
    m_environment->define(stmt.name.lexeme, value);

    // log environment state for debugging
    logState();
}

void Interpreter::visitWhileStmt(WhileStmt& stmt) {
    auto val  = evaluate(stmt.condition);
    while (isTruthy(val)) {
        try {
            execute(stmt.body);
            val  = evaluate(stmt.condition);
            // Note: catching must be by reference, not by value
        } catch(Jump& jmp) {
            if (jmp.keyword.lexeme == "break") break;
            if (jmp.keyword.lexeme == "continue") continue;
        }

    }

}

TObject Interpreter::visitAssignExpr(AssignExpr& expr) {
    TObject value = evaluate(expr.value);
    // search the variable in locals map, if not, search in the globals map.
    auto iter = m_locals.find(expr.id());
    if (iter != m_locals.end()) {
      auto val = std::make_shared<TObject>(value);
      m_environment->assignAt(iter->second, expr.name, val);
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
    logMsg("\nIn visitcallExpr: "); 
    auto callee = evaluate(expr.callee);
    logMsg("callee: ", callee);
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
        msg,"Expected ",func->arity() 
           ," arguments but got " 
           ,v_args.size(),".";
        throw RuntimeError(expr.paren, msg.str());
    }
    logMsg("func->toString : ",func->toString());
    logMsg("func.use_count: ", func.use_count());

    logMsg("\nExit out visitcallExpr, before returns func->call:  "); 
    return func->call(*this, v_args);
}

TObject Interpreter::visitGetExpr(GetExpr& expr) {
  logMsg("\nIn visitGetExpr, name: ", expr.m_name);
  auto obj = evaluate(expr.m_object);
  logMsg("obj: ", obj, ", type: ", obj.getType(), ", id: ", obj.getId());
  if (obj.isInstance()) {
    logMsg("obj is an instance");
    // obj_ptr is the method
    auto obj_ptr = obj.getInstance()->get(expr.m_name);
    // Note: shared_ptr.get() returns the stored pointer, not the managed pointer.
    // *shared_ptr dereference the smart pointer
    logMsg("In interpreter getexpr: *obj_ptr: ", *obj_ptr);
  logMsg("\nExit out visitGetExpr, name, before returning obj_ptr");
    return *obj_ptr;
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

TObject Interpreter::visitSetExpr(SetExpr& expr) {
    logMsg("\nIn visitSet: ");
    logMsg("name: ", expr.m_name);
  auto obj = evaluate(expr.m_object);
  if (not obj.isInstance()) {
    throw RuntimeError(expr.m_name,
      "Only instances have fields.");
  }

  // TODO: evaluate function must returns lukobject with smart pointer
  auto value = evaluate(expr.m_value);

  auto val_ptr = std::make_shared<LukObject>(value);
  logMsg("value: ", value);
  logMsg("obj: ", obj, ", type: ", obj.getType());
  logMsg("obj.getId: ", obj.getId());
  auto instPtr = obj.getInstance();
  logMsg("instptr tostring: ", instPtr->toString());
  logMsg("set instance, name: ", expr.m_name, ", value: ", *val_ptr);
  instPtr->set(expr.m_name, val_ptr);
  logMsg("m_fields size from visitSet: ", instPtr->getFields().size());
  logMsg("Exit out visitSet: \n");
 
  return value;
}

TObject Interpreter::visitSuperExpr(SuperExpr& expr) {
  logMsg("\nIn visitSuperExpr: ");
  logMsg("expr.m_method: ", expr.m_method, ", expr.id: ", expr.id());
  auto iter = m_locals.find(expr.id());
  if (iter != m_locals.end()) {
    // iter->second is the depth
    int distance = iter->second;
    auto objClass = m_environment->getAt(distance, "super");
    // TODO: it will better to test whether is classable
    auto superclass = objClass.getDynCast<LukClass>();
    
    // "this" is always one level nearer than "super"'s environment.
    auto objInst = m_environment->getAt(
      distance - 1, "this");
    auto instPtr = objInst.getInstance();
    ObjPtr method = superclass->findMethod(expr.m_method.lexeme);
    if (method == nullptr) {
      throw RuntimeError(expr.m_method,
          "Undefined property '" + expr.m_method.lexeme + "'.");
    }

    std::shared_ptr<LukFunction> funcPtr = method->getDynCast<LukFunction>();
    logMsg("\nExit out visitSuperExpr before return  funtcPtr->bind");
    return *funcPtr->bind(instPtr);

  }

  return TObject();
}


TObject Interpreter::visitThisExpr(ThisExpr& expr) {
  logMsg("\nIn visitThis");
  logMsg("keyword: ", expr.m_keyword);
  auto obj = lookUpVariable(expr.m_keyword, expr);

  logMsg("Exit out visitThis\n");
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
    return lookUpVariable(expr.name, expr);
}

TObject Interpreter::lookUpVariable(Token& name, Expr& expr) {
  logMsg("\nIn lookUpVariable name: ", name.lexeme);
  // TODO: must be factorized
  // searching the depth in locals map
  // whether not, get the variable in globals map
  auto iter = m_locals.find(expr.id());
  if (iter != m_locals.end()) {
    // iter->second is the depth
    return m_environment->getAt(iter->second, name.lexeme);
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
    logMsg("\nIn stringify, obj: ", obj);
    if (obj.isNil() || obj.isBool()) return obj.value();
    if (obj.isNumber()) {
        std::string text = obj.value(); 
        std::string end = ".000000";
        // extract decimal part if ending by .0
        if (endsWith(text, end)) 
            return text.substr(0, text.size() - end.size());
        return text;
    } 
    
   
    logMsg("Exit out stringify \n");
    return obj.toString();
}


