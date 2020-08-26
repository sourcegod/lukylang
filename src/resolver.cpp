#include "resolver.hpp"
#include "interpreter.hpp"

Resolver::Resolver(Interpreter& interp, LukError& lukErr)
      : m_interp(interp),
      m_lukErr(lukErr) {
    logMsg("\nIn Resolver constructor");
}

void Resolver::beginScope() {
  std::unordered_map<std::string, bool> scope;
  m_scopes.push_back(scope);
}

void Resolver::endScope() {
  m_scopes.pop_back();
}

void Resolver::declare(TokPtr& name) {
  if (m_scopes.size() == 0) return;
  auto& scope = m_scopes.back();
  auto iter = scope.find(name->lexeme);
  if (iter != scope.end()) {
    m_lukErr.error(errTitle, name, "This Variable is allready declared in this scope.");
  }
  scope[name->lexeme] = false;

}

void Resolver::define(TokPtr& name) {
  if (m_scopes.size() == 0) return;
  auto& scope = m_scopes.back(); 
  scope.at(name->lexeme) = true;
}

// resolve vector
void Resolver::resolve(std::vector<std::shared_ptr<Stmt>>& statements) {
    if (statements.empty()) {
        m_lukErr.error(errTitle, "Vector is empty.\n");
        return;
    }
    
    for (auto& stmt : statements) {
        resolve(stmt);
    }
 
}

// resolve statement
void Resolver::resolve(StmtPtr& stmt) {
  stmt->accept(*this);
}

void Resolver::resolveFunction(FunctionExpr& func, FunctionType ft) {
  auto enclosingFt = m_curFunction;
  m_curFunction = ft;
  beginScope();
  for (TokPtr& param: func.m_params) {
    declare(param);
    define(param);
  }

  resolve(func.m_body);
  endScope();
  m_curFunction = enclosingFt;
}

// resolve expressions
void Resolver::resolve(ExprPtr expr) {
  expr->accept(*this);
}

void Resolver::resolveLocal(Expr* expr, TokPtr& name) {
  // FIX: why we cannot receive as argument an Expr& instead Expr* ???
  // because expr is a pointer object, and a non const object, 
  // so, we cannot pass as a constant (&) object.
  for (int i = m_scopes.size() -1; i >=0; --i) {
    auto& scope = m_scopes.at(i);
    auto iter = scope.find(name->lexeme);
    if (iter != scope.end()) {
      int val = m_scopes.size() -1 - i;
      m_interp.resolve(*expr, val);
    }
  }
  
  // Not found. Assume it is global
}

// expressions
ObjPtr Resolver::visitAssignExpr(AssignExpr& expr) {
    logMsg("\nIn visitAssignExpr, Resolver, name:  ", expr.m_name);
    resolve(expr.m_value);
    resolveLocal(&expr, expr.m_name);
  
  return nilptr;
}

ObjPtr Resolver::visitBinaryExpr(BinaryExpr& expr) {
  resolve(expr.m_left);
  resolve(expr.m_right);
  
  return nilptr;
}

ObjPtr Resolver::visitCallExpr(CallExpr& expr) {
  resolve(expr.m_callee);
  for (std::shared_ptr<Expr>& arg : expr.m_args) {
    resolve(arg);
  }

  return nilptr;
}

ObjPtr Resolver::visitFunctionExpr(FunctionExpr& expr) {
  resolveFunction(expr, FunctionType::Function);
  return nilptr;
}


ObjPtr Resolver::visitGetExpr(GetExpr& expr) {
  resolve(expr.m_object);

  return nilptr;
}

ObjPtr Resolver::visitGroupingExpr(GroupingExpr& expr) {
  resolve(expr.m_expression);
 
  return nilptr;
}

ObjPtr Resolver::visitLiteralExpr(LiteralExpr& expr) {
    logMsg("\nIn visitLiteralExpr, Resolver, value: ", expr.m_value->toString());

    return nilptr;
}

ObjPtr Resolver::visitLogicalExpr(LogicalExpr& expr) {
  resolve(expr.m_left);
  resolve(expr.m_right);
  
  return nilptr;
}

ObjPtr Resolver::visitSetExpr(SetExpr& expr) {
  resolve(expr.m_value);
  resolve(expr.m_object);

  return nilptr;
}

ObjPtr Resolver::visitSuperExpr(SuperExpr& expr) {
    if (currentClass == ClassType::None) {
      m_lukErr.error(errTitle, expr.m_keyword,
          "Cannot use 'super' outside of a class.");
    } else if (currentClass != ClassType::Subclass) {
      m_lukErr.error(errTitle, expr.m_keyword,
          "Cannot use 'super' in a class with no superclass.");
    }

  resolveLocal(&expr, expr.m_keyword);
  
  return nilptr;
}


ObjPtr Resolver::visitThisExpr(ThisExpr& expr) {
  if (currentClass == ClassType::None) {
      m_lukErr.error(errTitle, expr.m_keyword,
          "Cannot use 'this' outside of a class.");
    }

  resolveLocal(&expr, expr.m_keyword);

  return nilptr;
}

ObjPtr Resolver::visitUnaryExpr(UnaryExpr& expr) {
  resolve(expr.m_right);
  
  return nilptr;
}

ObjPtr Resolver::visitVariableExpr(VariableExpr& expr) {
  if (m_scopes.size() != 0) {
    auto& scope = m_scopes.back();
    auto iter = scope.find(expr.m_name->lexeme);
    if (iter != scope.end() && iter->second == false) {
      m_lukErr.error(errTitle, expr.m_name, "Cannot read local variable in its own initializer.");
    }
  }
  resolveLocal(&expr, expr.m_name);

  return nilptr;
}

// statements
void Resolver::visitBlockStmt(BlockStmt& stmt) {
  beginScope();
  resolve(stmt.m_statements);
  endScope();

}

void Resolver::visitBreakStmt(BreakStmt& stmt) {
}


void Resolver::visitClassStmt(ClassStmt& stmt) {
  ClassType enclosingClass = currentClass;
  currentClass = ClassType::Class;
  
  declare(stmt.m_name);
  define(stmt.m_name);

  if (stmt.m_superclass != nullptr &&
      stmt.m_name->lexeme == stmt.m_superclass->m_name->lexeme) {
      m_lukErr.error(errTitle, stmt.m_superclass->m_name,
        "A class cannot inherit from itself.");
  }

  if (stmt.m_superclass != nullptr) {
    // Note: changing resolve(ExprPtr&) to resolve(ExprPtr), to accept VariableExpr as parameter
    currentClass = ClassType::Subclass;
    resolve(stmt.m_superclass);
  }
  
  if (stmt.m_superclass != nullptr) {
    beginScope();
    if (m_scopes.size() == 0) return;
    auto& scope = m_scopes.back(); 
    scope["super"] = true;
  }
  
  beginScope();
  if (m_scopes.size() == 0) return;
  auto& scope = m_scopes.back(); 
  scope["this"] = true;

  for (auto funcStmt: stmt.m_methods) {
    auto declaration = FunctionType::Method;
    if (funcStmt->m_name->lexeme == "init") {
      declaration = FunctionType::Initializer;
    }
    resolveFunction(*funcStmt->m_function, declaration); // [local] 
  }

  endScope();
  if (stmt.m_superclass != nullptr) endScope();
  currentClass = enclosingClass;
}


void Resolver::visitExpressionStmt(ExpressionStmt& stmt) {
  resolve(stmt.m_expression);
}


void Resolver::visitFunctionStmt(FunctionStmt& stmt) {
  declare(stmt.m_name);
  define(stmt.m_name);
  resolveFunction(*stmt.m_function, FunctionType::Function);

}


void Resolver::visitIfStmt(IfStmt& stmt) {
  resolve(stmt.m_condition);
  resolve(stmt.m_thenBranch);
  if (stmt.m_elseBranch != nullptr) resolve(stmt.m_elseBranch);

}

void Resolver::visitPrintStmt(PrintStmt& stmt) {
  resolve(stmt.m_expression);

}

void Resolver::visitReturnStmt(ReturnStmt& stmt) {
  if (m_curFunction == FunctionType::None)
    m_lukErr.error(errTitle, stmt.name, "Cannot return from top-level code.");
  if (stmt.value != nullptr) {
    if (m_curFunction == FunctionType::Initializer) {
        m_lukErr.error(errTitle, stmt.name,
            "Cannot return a value from an initializer.");
    }
    
    resolve(stmt.value);
  }

}

void Resolver::visitVarStmt(VarStmt& stmt) {
  declare(stmt.name);
  if (stmt.initializer != nullptr) {
    resolve(stmt.initializer);
  }
  define(stmt.name);

}

void Resolver::visitWhileStmt(WhileStmt& stmt) {
  resolve(stmt.condition);
  resolve(stmt.body);

}

