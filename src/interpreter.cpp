#include "interpreter.hpp"
#include "lukerror.hpp"
#include "runtimeerror.hpp"
#include "jump.hpp"
#include "lukcallable.hpp"
#include "builtin_func.hpp"
#include "lukfunction.hpp"
#include "return.hpp"
#include "logger.hpp"
#include "lukclass.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo> // type name
#include <sstream> // for stringstream
#include <cmath> // for fmod
Interpreter::Interpreter(LukError& lukErr) : m_lukErr(lukErr) {
    logMsg("\nIn Interpreter constructor");
    LogConf.headers = true;
    LogConf.level = log_DEBUG;
    CLog(log_WARN) << "log_WARN: Test on log warning.";
    
    logMsg("\n--- Starts Interpreter");
    logTest();

    m_globals = std::make_shared<Environment>();
    m_env = m_globals;
    m_globals->m_name = "Globals, " + m_globals->m_name;
    m_result = nilptr;

    // TRACE_ALL;
    // TRACE_MSG("Env globals tracer: ");
    
    // native clock function
    auto clock_func = std::make_shared<ClockFunc>();
    m_globals->define("clock", std::make_shared<LukObject>(clock_func));

    // native println function
    auto println_func = std::make_shared<PrintlnFunc>();
    m_globals->define("println", std::make_shared<LukObject>(println_func));

    // native readln function
    auto readln_func = std::make_shared<ReadlnFunc>();
    m_globals->define("readln", std::make_shared<LukObject>(readln_func));


    logMsg("\nExit out Interpreter constructor");

}

void Interpreter::interpret(std::vector<std::shared_ptr<Stmt>> statements) {
    logMsg("\nIn Interpret, starts loop");

    if (statements.empty()) { 
        std::cerr << "Interp: vector is empty.\n";
    }
    logState();
    try {
         for (auto& stmt : statements) {
            if (stmt) {
                execute((stmt));
            }
        }
        
    // Note: passing exception by reference to avoid copy
    } catch (RuntimeError& err) {
        std::cerr << m_errTitle << err.what() << "\n";
    }

    assert(m_result != nullptr);
    if (m_result != nullptr && !m_result->isNil()) {
        printResult();
    }

    logState();

  logMsg("\nExit out  Interpret");

}

void Interpreter::printResult() {
    // CLog(log_DEBUG) << "printResult avant \n";
    std::cout << stringify(m_result) << "\n";
    // reinitialize m_result to nil
    m_result = nilptr;
    
}


void Interpreter::logState() {
#ifdef DEBUG
  logMsg("\nEnvironment state");
  // Note: workaround to make an alias for a variable in c++ 
  // int a; 
  // int* const b = &a;
  // b is an alias or pointer to a
  // but not work for a map
  logMsg("Globals state");
  auto& values = m_globals->getValues();
  // Note: Pattern: looping over map
  if (values.empty()) {
      logMsg("m_globals env is empty");
  } else {
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


void Interpreter::logTest() {
#ifdef DEBUG
  logMsg("\nIn logTest");
  
  /*
  // ObjPtr p_obj = std::make_shared<LukObject>();
  // ObjPtr p_obj = TObject::getNilPtr();
  ObjPtr p_obj = nilptr;
  // Note: cannot overloading operator= with nullptr
  // p_obj = nullptr;
  logMsg("p_obj: ", p_obj);
  logMsg("p_obj->toString: ", p_obj->toString());
  */



  logMsg("\nExit out logTest");
#endif
}

ObjPtr Interpreter::evaluate(ExprPtr expr) { 
    logMsg("\nIn evaluate, expr: ", typeid(*expr).name());
     auto obj = expr->accept(*this);
     if (obj == nullptr) {
       std::cerr << "Evaluated expr " << expr->typeName() << " to nullptr \n";
     }

    logMsg("Evaluating obj result after accept: ", obj->toString());
    return obj;
}

void Interpreter::execute(StmtPtr& stmt) {
    logMsg("\nIn execute top level, *stmt: ", typeid(*stmt).name());
    stmt->accept(*this);
}

ObjPtr Interpreter::visitAssignExpr(AssignExpr& expr) {
    logMsg("\nIn visitAssignExpr Interpreter, name:  ", expr.m_name);
    ObjPtr value = evaluate(expr.m_value);
    ObjPtr cur = m_env->get(expr.m_name);
    // std::cerr << "cur: " << cur << ", value: " << value << "\n";
    auto op = expr.m_equals;
    /// Note: In C++, switch statement is fallthrough by default, so, you should put a
    /// [break] keyword to indicate that statement not fallthrough.
    switch(expr.m_equals->type) {
      case TokenType::EQUAL: break;
      case TokenType::PLUS_EQUAL:
          if (cur->isNumber() && value->isNumber()) {
              *value = *cur + *value;
          } else if ( (value->isString() && cur->isString())  ||
              (value->isString() && cur->isNumeric()) || 
              (value->isNumeric() && cur->isString()) ) {
              // Note: temporary can concatenate string with number before having number to string convertion function
              *value = format(cur) + format(value);
              // *value = *cur + *value;
          }
          else throw RuntimeError(op, 
                  "Operands must be string and number.");
          break;
      
      case TokenType::MINUS_EQUAL:
          checkNumberOperands(op, cur, value);
          *value = *cur - *value;
          break;

      case TokenType::STAR_EQUAL:
          if ( (cur->isNumber()) && (value->isNumber()) ) {
              *value *= *cur;
          } else if ( cur->isString() && value->isNumber() ) { 
              // Note: can multiply string by number
              if ( not value->isInt()) {
                  throw RuntimeError(op, "String multiplier must be an integer");
              }
              auto str = cur->getString();
              auto num = value->getNumber();
               
              *value = multiplyString(str, num);
          } else if ( cur->isNumber() && value->isString() ) { 
              if ( not cur->isInt()) {
                  throw RuntimeError(op, "String multiplier must be an integer");
              }
              auto str = value->getString();
              auto num = cur->getNumber();
              
              *value = multiplyString(str, num);
          }
          else throw RuntimeError(op, "Operands must be strings or numbers.");
          break;

      case TokenType::SLASH_EQUAL:
          checkNumberOperands(op, cur, value);
          *value = *cur / *value;
          break;

      case TokenType::MOD_EQUAL:
          checkNumberOperands(op, cur, value);
          *value = *cur % *value;
          break;

      case TokenType::EXP_EQUAL:
          // Note: pow function returns double
          // so, you must convert it to Int ingegral operands
          checkNumberOperands(op, cur, value);
          if ( cur->isInt() && value->isInt() &&
                  value->getInt() >= 0 ) {
              *value = int( std::pow( cur->getNumber(), value->getNumber()) );
          } 
          else *value = std::pow( cur->getNumber(), value->getNumber() );
          break;

       // bitwise operators compound assignment
       case TokenType::BIT_OR_EQUAL:
            if (cur->isBoolInt() && value->isBoolInt() )
                *value = *cur | *value;
            else throw RuntimeError(op, "operands must be bools or integers.");
            break;

       case TokenType::BIT_AND_EQUAL:
            if (cur->isBoolInt() && value->isBoolInt() )
                *value = *cur & *value;
            else throw RuntimeError(op, "operands must be bools or integers.");
            break;

       case TokenType::BIT_XOR_EQUAL:
            if (cur->isBoolInt() && value->isBoolInt() )
                *value = *cur ^ *value;
            else throw RuntimeError(op, "operands must be bools or integers.");
            break;

       case TokenType::BIT_LEFT_EQUAL:
            if (cur->isBoolInt() && value->isBoolInt() )
                *value = *cur << *value;
            else throw RuntimeError(op, "operands must be bools or integers.");
            break;

       case TokenType::BIT_RIGHT_EQUAL:
            if (cur->isBoolInt() && value->isBoolInt() )
                *value = *cur >> *value;
            else throw RuntimeError(op, "operands must be bools or integers.");
            break;

      default: break;
    }

    // search the variable in locals map, if not, search in the globals map.
    auto iter = m_locals.find(expr.id());
    if (iter != m_locals.end()) {
      m_env->assignAt(iter->second, expr.m_name, value);
    } else {
      m_globals->assign(expr.m_name, value);
    }
    
    return value;
}

ObjPtr Interpreter::visitBinaryExpr(BinaryExpr& expr) {
    // Note: the method .get allow to convert smart pointer to raw pointer
    logMsg("\nIn visitBinary: "); 
    ObjPtr left = evaluate(expr.m_left);
    ObjPtr right = evaluate(expr.m_right);
    logMsg("left: ", left->toString(), ", operator: ", expr.m_op->lexeme, ", right: ", right->toString());
    switch(expr.m_op->type) {
        case TokenType::PLUS:
            if (left->isNumber() && right->isNumber()) {
                return std::make_shared<LukObject>( *left + *right );
            }
            
            // Note: temporary can concatenate string with number before having number to string convertion function
            if ( (left->isString() && right->isString())  ||
                (left->isString() && right->isNumeric()) || 
                (left->isNumeric() && right->isString()) )
                return std::make_shared<LukObject>( format(left) + format(right) );
            throw RuntimeError(expr.m_op, 
                    "Operands must be string and number.");
        
        case TokenType::MINUS:
            checkNumberOperands(expr.m_op, left, right);
            return std::make_shared<LukObject>(*left - *right);
 
        case TokenType::STAR:
            if (left->isNumber() && right->isNumber())
              return std::make_shared<LukObject>(*left * *right);

            // Note: can multiply string by number
            if ( left->isString() && right->isNumber() ) { 
                if ( not right->isInt()) {
                    // if (std::fmod(nb, 1) != 0) 
                    throw RuntimeError(expr.m_op,
                        "String multiplier must be an integer");
                }
                auto str = left->getString();
                auto num = right->getNumber();
                 
                return std::make_shared<LukObject>( multiplyString(str, num) );
            } else if ( left->isNumber() && right->isString() ) { 
                if ( not left->isInt()) {
                    throw RuntimeError(expr.m_op,
                        "String multiplier must be an integer");
                }
                auto str = right->getString();
                auto num = left->getNumber();
                
                return std::make_shared<LukObject>( multiplyString(str, num) );
            }
            
            throw RuntimeError(expr.m_op, "Operands must be strings or numbers.");

        case TokenType::SLASH:
            checkNumberOperands(expr.m_op, left, right);
            return std::make_shared<LukObject>(*left / *right);
       
        case TokenType::MOD:
            checkNumberOperands(expr.m_op, left, right);
            // Note: cannot use modulus % on double
            // use instead fmod function for modulus between double
            return std::make_shared<LukObject>( *left %  *right);

        case TokenType::EXP:
            checkNumberOperands(expr.m_op, left, right);
            // Note: pow function returns double
            // so, you must convert it to Int ingegral operands
            if ( left->isInt() && right->isInt() &&
                    right->getInt() >= 0 )
                return std::make_shared<LukObject>( int(std::pow( left->getNumber(), right->getNumber()) ));
            return std::make_shared<LukObject>(std::pow( left->getNumber(), right->getNumber() ));
  
        case TokenType::GREATER:
            // checkNumberOperands(expr.m_op, left, right);
            return std::make_shared<LukObject>(*left > *right);
        
        case TokenType::GREATER_EQUAL:
            // checkNumberOperands(expr.m_op, left, right);
            return std::make_shared<LukObject>(*left >= *right);

        case TokenType::LESSER:
            // checkNumberOperands(expr.m_op, left, right);
            return std::make_shared<LukObject>(*left < *right);

        case TokenType::LESSER_EQUAL:
            // checkNumberOperands(expr.m_op, left, right);
            return std::make_shared<LukObject>(*left <= *right);
            
   
        case TokenType::BANG_EQUAL: return std::make_shared<LukObject>(*left != *right);
        case TokenType::EQUAL_EQUAL: return std::make_shared<LukObject>(*left == *right);

         // Adding: bitwise operators
        case TokenType::BIT_OR:
            if (left->isBoolInt() && right->isBoolInt() )
                return std::make_shared<LukObject>(*left | *right);
            throw RuntimeError(expr.m_op, "operands must be bools or integers.");

        case TokenType::BIT_AND:
            if (left->isBoolInt() && right->isBoolInt() )
                return std::make_shared<LukObject>(*left & *right);
            throw RuntimeError(expr.m_op, "operands must be bools or integers.");

        case TokenType::BIT_XOR:
            if (left->isBoolInt() && right->isBoolInt() )
                return std::make_shared<LukObject>(*left ^ *right);
            throw RuntimeError(expr.m_op, "operands must be bools or integers.");

        case TokenType::BIT_LEFT:
            if (left->isBoolInt() && right->isBoolInt() )
                return std::make_shared<LukObject>(*left << *right);
            throw RuntimeError(expr.m_op, "operands must be bools or integers.");

        case TokenType::BIT_RIGHT:
            if (left->isBoolInt() && right->isBoolInt() )
                return std::make_shared<LukObject>(*left >> *right);
            throw RuntimeError(expr.m_op, "operands must be bools or integers.");
         
        // comma operator
        case TokenType::COMMA:
            return right;

        default: break;
    }


    // unrichable
    return nilptr;
}

ObjPtr Interpreter::visitCallExpr(CallExpr& expr) {
    logMsg("\nIn visitcallExpr: ", typeid(expr).name()); 
    auto callee = evaluate(expr.m_callee);
    logMsg("Still In visitCallExpr, callee: ", callee);
    if (! callee->isCallable()) {
      logMsg("voici calle: id", callee->id, ", string: ", callee->toString());
      throw RuntimeError(expr.m_paren, "Can only call function and class.");
    }

    std::vector<ObjPtr> v_args;
    for (auto& arg: expr.m_args) {
        v_args.push_back(evaluate(arg));
    }
    const auto& func = callee->getCallable();
    
    /// Note: 255 arguments means variadic function
    if (func->arity() != 255 && v_args.size() != func->arity()) {
        std::ostringstream msg;
        msg << "Expected " << func->arity() 
           << " arguments but got " 
           << v_args.size() << ".";
        throw RuntimeError(expr.m_paren, msg.str());
    }
    logMsg("func->toString : ",func->toString());
    logMsg("func.use_count: ", func.use_count());

    logMsg("\nExit out visitcallExpr, before returns func->call:  "); 
    return func->call(*this, v_args);
}

ObjPtr Interpreter::visitFunctionExpr(FunctionExpr& expr) {
  logMsg("\nIn visitFunctionExpr, id: ", expr.id());
  // Note: lambda function not need to be in the environment stack
  auto exprP = std::make_shared<FunctionExpr>(expr);
  auto funcPtr = std::make_shared<LukFunction>("", exprP, m_env, false);
  ObjPtr objP = std::make_shared<LukObject>(funcPtr);

  return objP; 
}


ObjPtr Interpreter::visitGetExpr(GetExpr& expr) {
  logMsg("\nIn visitGetExpr, name: ", expr.m_name);
  auto obj = evaluate(expr.m_object);
  logMsg("obj: ", obj, ", type: ", obj->getType(), ", id: ", obj->getId());
  if (obj->isInstance()) {
    logMsg("obj is an instance");
    // obj_ptr is the method
    auto obj_ptr = obj->getInstance()->get(expr.m_name);
    // Note: shared_ptr.get() returns the stored pointer, not the managed pointer.
    // *shared_ptr dereference the smart pointer
    // so, after *shared_ptr, you cannot use it again.
    // so, dont use *shared_ptr
    logMsg("In visitGetExpr, obj_ptr: ", obj_ptr->toString());
  logMsg("\nExit out visitGetExpr, name, before returning obj_ptr");
    return obj_ptr;
  }

  throw RuntimeError(expr.m_name,
    "Only instances have properties.");
  
  return nilptr;
}

ObjPtr Interpreter::visitGroupingExpr(GroupingExpr& expr) {
    return evaluate(expr.m_expression);
}

ObjPtr Interpreter::visitLiteralExpr(LiteralExpr& expr) {
    logMsg("\nIn visitLiteralExpr Interpreter, value: ", expr.m_value->toString());
    return expr.m_value;
}


ObjPtr Interpreter::visitLogicalExpr(LogicalExpr& expr) {
    ObjPtr left = evaluate(expr.m_left);
    if (expr.m_op->type == TokenType::OR) {
        if (isTruthy(left)) return left;

    } else {
        if (!isTruthy(left)) return left;
    }
    
    return evaluate(expr.m_right);
}

ObjPtr Interpreter::visitSetExpr(SetExpr& expr) {
    logMsg("\nIn visitSet: ");
    logMsg("name: ", expr.m_name);
  auto objP = evaluate(expr.m_object);
  if (not objP->isInstance()) {
    throw RuntimeError(expr.m_name,
      "Only instances have fields.");
  }

  auto value = evaluate(expr.m_value);
  logMsg("value: ", value);
  logMsg("obj: ", objP, ", type: ", objP->getType());
  logMsg("obj->getId: ", objP->getId());
  auto instPtr = objP->getInstance();
  logMsg("instptr tostring: ", instPtr->toString());
  logMsg("Set instance, name: ", expr.m_name, ", value: ", value);
  instPtr->set(expr.m_name, value);
  logMsg("m_fields size from visitSet: ", instPtr->getFields().size());
  logMsg("Exit out visitSet: \n");
 
  return value;
}

ObjPtr Interpreter::visitSuperExpr(SuperExpr& expr) {
  logMsg("\nIn visitSuperExpr: ");
  logMsg("expr.m_method: ", expr.m_method, ", expr.id: ", expr.id());
  auto iter = m_locals.find(expr.id());
  if (iter != m_locals.end()) {
    // iter->second is the depth
    int distance = iter->second;
    auto objClass = m_env->getAt(distance, "super");
    // TODO: it will better to test whether is classable
    auto superclass = objClass->getDynCast<LukClass>();
    
    // "this" is always one level nearer than "super"'s environment.
    auto objInst = m_env->getAt(
      distance - 1, "this");
    auto instPtr = objInst->getInstance();
    ObjPtr method = superclass->findMethod(expr.m_method->lexeme);
    if (method == nullptr) {
      throw RuntimeError(expr.m_method,
          "Undefined property '" + expr.m_method->lexeme + "'.");
    }

    std::shared_ptr<LukFunction> funcPtr = method->getDynCast<LukFunction>();
    logMsg("\nExit out visitSuperExpr before return  funtcPtr->bind");
    return funcPtr->bind(instPtr);

  }

  return nilptr;
}

ObjPtr Interpreter::visitTernaryExpr(TernaryExpr& expr) {
    auto val = evaluate(expr.m_condition);
    if (isTruthy(val)) {
      return evaluate(expr.m_thenBranch);
    }
    
    return evaluate(expr.m_elseBranch);
}


ObjPtr Interpreter::visitThisExpr(ThisExpr& expr) {
  logMsg("\nIn visitThis");
  logMsg("keyword: ", expr.m_keyword);
  auto obj = lookUpVariable(expr.m_keyword, expr);

  logMsg("Exit out visitThis\n");
  return obj;
}

ObjPtr Interpreter::visitUnaryExpr(UnaryExpr& expr) {
    ObjPtr right = evaluate(expr.m_right);
    switch(expr.m_op->type) {
        case TokenType::BANG:
            return std::make_shared<LukObject>(!isTruthy(right));
        
        case TokenType::MINUS:
            checkNumberOperand(expr.m_op, right);
            return std::make_shared<LukObject>(-*right);

        case TokenType::PLUS:
            checkNumberOperand(expr.m_op, right);
            return right; // std::make_shared<LukObject>(*right);
        
        // bitwise NOT operator
        case TokenType::BIT_NOT:
            checkNumberOperand(expr.m_op, right);
            return std::make_shared<LukObject>(~*right);

        // prefix, postfix operators
        /// Note: prefix operator assign the new value to the variable, and returning it after.
        /// but postfix operator, returns the variable, and assign the new value after.
        case TokenType::MINUS_MINUS:
            if (expr.m_right->isVariableExpr()) {
                checkNumberOperand(expr.m_op, right);
                /// Note: creating an object with value 1, to be able to make operations between objects
                auto objVal = LukObject(1);
                auto var = expr.m_right;
                auto name = var->getName(); 
                auto objP = std::make_shared<LukObject>(*right - objVal);
                m_env->assign(name, objP);
                if (expr.m_isPostfix) return right;
                else return std::make_shared<LukObject>(*right - objVal);
            }
            throw RuntimeError(expr.m_op,
                "Operand of a decrement operator must be a variable.");

        case TokenType::PLUS_PLUS:
            if (expr.m_right->isVariableExpr()) {
                checkNumberOperand(expr.m_op, right);
                auto objVal = LukObject(1);
                auto var = expr.m_right;
                auto name = var->getName(); 
                auto objP = std::make_shared<LukObject>(*right + objVal);
                m_env->assign(name, objP);
                if (expr.m_isPostfix) return right;
                else return std::make_shared<LukObject>(*right + objVal);
            }
            throw RuntimeError(expr.m_op,
                "Operand of a increment operator must be a variable.");
          

        default: break;
    }

    return nilptr;
}

ObjPtr Interpreter::visitVariableExpr(VariableExpr& expr) {
  logMsg("\nIn visitVariableExpr, name:   ", expr.m_name);
  return lookUpVariable(expr.m_name, expr);
}

ObjPtr Interpreter::lookUpVariable(TokPtr& name, Expr& expr) {
  logMsg("\nIn lookUpVariable name: ", name->lexeme);
  // searching the depth in locals map
  // whether not, get the variable in globals map
  auto iter = m_locals.find(expr.id());
  if (iter != m_locals.end()) {
    // iter->second is the depth
    return m_env->getAt(iter->second, name->lexeme);
  }

  return m_globals->get(name);
}

bool Interpreter::isTruthy(ObjPtr& obj) {
    if (obj->isNil()) return false;
    if (obj->isBool()) return obj->getBool();
    if (obj->isInt() && obj->getInt() == 0) return false;
    if (obj->isNumber() && obj->getNumber() == 0) return false;
    if (obj->isString() && obj->getString() == "") return false;
    
    return true;
}

bool Interpreter::isEqual(ObjPtr& a, ObjPtr& b) {
    // nil is only equal to nil
    if (a->isNil() && b->isNil()) return true;
    if (a->isNil()) return false;

    return *a == *b;
}

void Interpreter::checkNumberOperand(TokPtr& op, ObjPtr& operand) {
    if (operand->isBool() || operand->isNumber()) return;
    throw RuntimeError(op, "Operand must be bool or number.");
}

void Interpreter::checkNumberOperands(TokPtr& op, ObjPtr& left, ObjPtr& right) {
    if (left->isNumber() && right->isNumber()) return;
    throw RuntimeError(op, "Operands must be numbers.");
}


void Interpreter::resolve(Expr& expr, int depth) {
  logMsg("\nIn Resolve expr, Interpreter");
  // Note: FIX: abstract class Expr cannot be in map
  // so, we store its uniq id in the map
  m_locals[expr.id()] = depth;
}


void Interpreter::executeBlock(std::vector<StmtPtr>& statements, EnvPtr env) {
    logMsg("\nIn ExecuteBlock: ");
    auto previous = m_env;
    try {
        m_env = env;
        for (auto& stmt: statements) {
            if (stmt)
                // not use move because for reuse of the block
                // stmt->accept(*this);
                execute(stmt);

        }

    // Note: must catch all exceptions, even the Return exception.
    } catch(...) {
        m_env = previous;
        // throw up the exception
        throw;
    }
    // finally, whether no exception
    m_env = previous;
    // reset global variable m_result
    m_result = nilptr;
    
    logMsg("\nExit out  ExecuteBlock: ");
}

void Interpreter::visitBlockStmt(BlockStmt& stmt) {
    executeBlock(stmt.m_statements, std::make_shared<Environment>(m_env) );
}

void Interpreter::visitBreakStmt(BreakStmt& stmt) {
    std::string msg;
    if (stmt.m_keyword->lexeme == "break") {
        msg = "Error: 'break' must with while loop";
        throw Jump(stmt.m_keyword, msg);
    } else if (stmt.m_keyword->lexeme == "continue") {
        msg = "Error: 'continue' must with while loop";
        throw Jump(stmt.m_keyword, msg);
    }
         
}

void Interpreter::visitClassStmt(ClassStmt& stmt) {
  logMsg("In visitClassStmt: name: ", stmt.m_name->lexeme);
  ObjPtr superclass = nilptr;
  std::shared_ptr<LukClass> supKlass = nullptr;
  if (stmt.m_superclass != nullptr) {
    // Note: changing evaluate(ExprPtr&) to evaluate(ExprPtr) to passing VariableExpr object
    superclass = evaluate(stmt.m_superclass);
    logMsg("superclass: ", superclass);
    // TODO: It will better to test whether superclass is classable instead callable
    if (!superclass->isCallable()) { //  instanceof LoxClass)) {
      throw RuntimeError(stmt.m_superclass->m_name,
            "Superclass must be a class.");
    } else {
      supKlass = superclass->getDynCast<LukClass>();
    }

  }

  m_env->define(stmt.m_name->lexeme, nilptr);

  if (stmt.m_superclass != nullptr) {
    m_env = std::make_shared<Environment>(m_env);
    m_env->define("super", superclass);

  }

  std::unordered_map<std::string, ObjPtr> methods;
  for (auto meth: stmt.m_methods) {
    auto func = std::make_shared<LukFunction>(meth->m_name->lexeme, 
        meth->m_function, m_env,
        meth->m_name->lexeme == "init");
    logMsg("func name: ", func->toString());
    auto obj_ptr = std::make_shared<LukObject>(func);
    logMsg("obj_ptr type: ", obj_ptr->getType());
    logMsg("ObjPtr callable: ", obj_ptr->getCallable()->toString());
    logMsg("Adding meth to methods map: ", meth->m_name->lexeme);
    methods[meth->m_name->lexeme] = obj_ptr;
  }

  auto klass = std::make_shared<LukClass>(stmt.m_name->lexeme, supKlass, methods);
  if (stmt.m_superclass != nullptr) {
    // Note: moving m_enclosing from private to public in Environment object
    m_env = m_env->m_enclosing;
  }
  logMsg("Assign klass: ", stmt.m_name, ", to m_env");
  m_env->assign(stmt.m_name, klass);
logMsg("Exit out visitClassStmt\n");
}

void Interpreter::visitExpressionStmt(ExpressionStmt& stmt) {
    m_result = evaluate(stmt.m_expression);
}

void Interpreter::visitFunctionStmt(FunctionStmt& stmt) {
    auto func = std::make_shared<LukFunction>(stmt.m_name->lexeme, 
        stmt.m_function, 
        m_env, false);
    ObjPtr objP = std::make_shared<LukObject>(func);
    m_env->define(stmt.m_name->lexeme, objP);
    logMsg("FunctionExpr use_count: ", stmt.m_function.use_count());
    
}

void Interpreter::visitIfStmt(IfStmt& stmt) {
    auto val  = evaluate(stmt.m_condition);
    if (isTruthy(val)) {
        // Note: no moving statement pointer, because
        // it will be necessary use later, for function call
        execute(stmt.m_thenBranch);
    } else if (stmt.m_elseBranch != nullptr) {
        execute(stmt.m_elseBranch);
    }

}

void Interpreter::visitPrintStmt(PrintStmt& stmt) {
    ObjPtr value = evaluate(stmt.m_expression);
    // Note: printing obj->toString instead *obj pointer
    // to avoid multiple object's destructors 
    logMsg("\nIn visitprint: id: ", value->getId(), ", value: ", value->toString());
    std::cout << stringify(value) << std::endl;
    m_result = nilptr;

}

void Interpreter::visitReturnStmt(ReturnStmt& stmt) {
    ObjPtr value = nilptr;
    if (stmt.m_value != nullptr) { 
        value = evaluate(stmt.m_value);
    }
       
    throw Return(value);
}


void Interpreter::visitVarStmt(VarStmt& stmt) {
    // Note: new ObjPtr needs to be initialized to nilptr to avoid crash
    ObjPtr value = nilptr;
    if (stmt.m_initializer != nullptr) {
        value = evaluate(stmt.m_initializer);
    }
    m_env->define(stmt.m_name->lexeme, value);

    // log environment state for debugging
    logState();
}

void Interpreter::visitWhileStmt(WhileStmt& stmt) {
    auto val  = evaluate(stmt.m_condition);
    while (isTruthy(val)) {
        try {
            execute(stmt.m_body);
            val  = evaluate(stmt.m_condition);
            // Note: catching must be by reference, not by value
        } catch(Jump& jmp) {
            if (jmp.m_keyword->lexeme == "break") break;
            if (jmp.m_keyword->lexeme == "continue") continue;
        }

    }

}

std::string Interpreter::multiplyString(const std::string& str, const int num) {
    std::string result = "";
    for (int i=0; i < num; i++) {
        result += str;
    }

    return result;
}

std::string Interpreter::format(ObjPtr& obj) { 
  return stringify(obj);
}

std::string Interpreter::stringify(ObjPtr& obj) { 
    logMsg("\nIn stringify, obj id: ", obj->getId(), ", val: ", obj->toString());
    // if (obj->isNil() || obj->isBool()) return obj->toString();
    if (obj->isDouble()) {
        std::string str = obj->toString(); 
        // erasing trailing zeros
        auto pos = str.find_last_not_of('0');
        // keeping the first zero whether they are only zeros after the dot
        if (str[pos] == '.') str.erase(pos +2, std::string::npos);
        else str.erase(pos +1, std::string::npos);
        
        return str;
     } 
    
   
    logMsg("\nExit out stringify \n");
    return obj->toString();
}

