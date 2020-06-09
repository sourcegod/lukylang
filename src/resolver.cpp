#include "resolver.hpp"
#include "interpreter.hpp"

Resolver::Resolver(Interpreter& interp, LukError& lukErr)
  : m_interp(interp)
  ,m_lukErr(lukErr)
  {}

void Resolver::beginScope() {
  logMsg("Resolver BeginScope");
  std::unordered_map<std::string, bool> scope;
  m_scopes.push_back(scope);
}

void Resolver::endScope() {
  logMsg("Resolver EndScope");
  m_scopes.pop_back();
}

void Resolver::declare(Token name) {
  // TODO: pass token by pointer
  if (m_scopes.size() == 0) return;
  auto& scope = m_scopes.back();
  auto elem = scope.find(name.lexeme);
  if (elem != scope.end()) {
    m_lukErr.error(errTitle, name, "This Variable is allready declared in this scope.");
  }
  scope[name.lexeme] = false;
  logMsg("Resolver declare: ", name, ", ", "false");

}

void Resolver::define(Token& name) {
  if (m_scopes.size() == 0) return;
  auto& scope = m_scopes.back(); 
  scope.at(name.lexeme) = true;
  logMsg("Resolver define: ", name, ", ", "true");

}

// resolve vector
void Resolver::resolve(std::vector<std::unique_ptr<Stmt>>& statements) {
  logMsg("\n--- Starts Resolver");
    if (statements.empty()) {
        m_lukErr.error(errTitle, "Vector is empty.\n");
        return;
    }
    
    for (auto& stmt : statements) {
        resolve(stmt);
    }
  logMsg("\n--- End Resolver");
    
 
}


// resolve statement
void Resolver::resolve(PStmt& stmt) {
  stmt->accept(*this);

}

void Resolver::resolveFunction(FunctionStmt& func, FunctionType ft) {
  logMsg("\n--- Resolver  Starts Function");
  auto enclosingFt = m_curFunction;
  m_curFunction = ft;
  beginScope();
  for (Token& param: func.params) {
    logMsg("Func param: ", param.lexeme);
    declare(param);
    define(param);
  }

  resolve(func.body);
  endScope();
  m_curFunction = enclosingFt;
  logMsg("\n--- Resolver End function\n");
}

// resolve expressions
void Resolver::resolve(PExpr& expr) {
  expr->accept(*this);
}

void Resolver::resolveLocal(Expr* expr, Token name) {
  // TODO: pass token by pointer
  for (int i = m_scopes.size() -1; i >=0; --i) {
    auto& scope = m_scopes.at(i);
    auto elem = scope.find(name.lexeme);
    if (elem != scope.end()) {
      int val = m_scopes.size() -1 - i;
      logMsg("resolve local: expr name: ", name, 
          ", typename: ", expr->typeName());
      m_interp.resolve(*expr, val);
    }
  }
  
  // Not found. Assume it is global
}

// expressions
TObject Resolver::visitAssignExpr(AssignExpr& expr) {
  logMsg("Resolve assign: name: ", expr.name,
      ", value: ", expr.typeName());
  resolve(expr.value);
  resolveLocal(&expr, expr.name);
  
  return TObject();
}

TObject Resolver::visitBinaryExpr(BinaryExpr& expr) {
  resolve(expr.left);
  resolve(expr.right);
  
  return TObject();
}

TObject Resolver::visitCallExpr(CallExpr& expr) {
  logMsg("Resolver callExpr: ", expr.typeName());
  resolve(expr.callee);
  for (std::unique_ptr<Expr>& arg : expr.args) {
    resolve(arg);
  }

  return TObject();
}

TObject Resolver::visitGroupingExpr(GroupingExpr& expr) {
  resolve(expr.expression);
 
  return TObject();
}

TObject Resolver::visitLiteralExpr(LiteralExpr& expr) {
   logMsg("Resolve literalExpr: ", expr.value);
  return TObject();
}

TObject Resolver::visitLogicalExpr(LogicalExpr& expr) {
  resolve(expr.left);
  resolve(expr.right);
  
  return TObject();
}

TObject Resolver::visitUnaryExpr(UnaryExpr& expr) {
  resolve(expr.right);
  
  return TObject();
}

TObject Resolver::visitVariableExpr(VariableExpr& expr) {
  logMsg("Resolver VariableExpr: ", expr.name);
  if (m_scopes.size() != 0) {
    auto& scope = m_scopes.back();
    auto elem = scope.find(expr.name.lexeme);
    if (elem != scope.end() && elem->second == false) {
      m_lukErr.error(errTitle, expr.name, "Cannot read local variable in its own initializer.");
    }
  }
  resolveLocal(&expr, expr.name);

  return TObject();
}

// statements
void Resolver::visitBlockStmt(BlockStmt& stmt) {
  beginScope();
  resolve(stmt.statements);
  endScope();

}

void Resolver::visitBreakStmt(BreakStmt& stmt) {
}


void Resolver::visitExpressionStmt(ExpressionStmt& stmt) {
  resolve(stmt.expression);

}

void Resolver::visitIfStmt(IfStmt& stmt) {
  resolve(stmt.condition);
  resolve(stmt.thenBranch);
  if (stmt.elseBranch != nullptr) resolve(stmt.elseBranch);

}

void Resolver::visitFunctionStmt(FunctionStmt* stmt) {
  declare(stmt->name);
  define(stmt->name);
  resolveFunction(*stmt, FunctionType::Function);

}

void Resolver::visitPrintStmt(PrintStmt& stmt) {
  resolve(stmt.expression);

}

void Resolver::visitReturnStmt(ReturnStmt& stmt) {
  if (m_curFunction == FunctionType::None)
    m_lukErr.error(errTitle, stmt.name, "Cannot return from top-level code.");
  if (stmt.value != nullptr) resolve(stmt.value);

}

void Resolver::visitVarStmt(VarStmt& stmt) {
  auto expr = stmt.initializer.get();
  logMsg("Resolver VarStmt, name: ", stmt.name, ", val: ", expr->typeName());
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

