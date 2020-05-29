#include "resolver.hpp"
#include "interpreter.hpp"

Resolver::Resolver(Interpreter& interp, LukError& lukErr)
  : m_interp(interp)
  ,m_lukErr(lukErr)
  {}

void Resolver::beginScope() {
  std::unordered_map<std::string, bool> scope;
  m_scopes.push_back(scope);
}

void Resolver::endScope() {
  m_scopes.pop_back();
}

void Resolver::declare(Token name) {
  // TODO: pass token by pointer
  if (m_scopes.size() == 0) return;
  auto& scope = m_scopes.back();
  auto elem = scope.find(name.lexeme);
  if (elem != scope.end()) {
    m_lukErr.error(errTitle, "This Variable is allready declared in this scope.");
  }
  scope[name.lexeme] = false;

}

void Resolver::define(Token& name) {
  if (m_scopes.size() == 0) return;
  auto& scope = m_scopes.back(); 
  scope.at(name.lexeme) = true;

}

// resolve vector
void Resolver::resolve(std::vector<std::unique_ptr<Stmt>>& statements) {
    if (statements.empty()) {
        m_lukErr.error(errTitle, "Vector is empty.\n");
        return;
    }
    for (auto& stmt : statements) {
        resolve(*stmt);
    }
 
}


// resolve statement
void Resolver::resolve(Stmt& stmt) {
  stmt.accept(*this);

}

// resolve expressions
void Resolver::resolve(Expr& expr) {
  expr.accept(*this);
}

void Resolver::resolveLocal(Expr* expr, Token name) {
  // TODO: pass token by pointer
  for (int i = m_scopes.size() -1; i >=0; --i) {
    auto& scope = m_scopes.at(i);
    auto elem = scope.find(name.lexeme);
    if (elem != scope.end()) {
      int val = m_scopes.size() -1 - i;
      // m_interp.resolve(expr, val);
    }
  }
  // Not found. Assume it is global
}

// expressions
TObject Resolver::visitVariableExpr(VariableExpr& expr) {
  if (m_scopes.size() != 0) {
    auto& scope = m_scopes.back();
    auto elem = scope.find(expr.name.lexeme);
    if (elem != scope.end() && elem->second == false) {
      m_lukErr.error(errTitle, expr.name, "Cannot read local variable in its own initializer.");
    }
  }
  resolveLocal(&expr, expr.name);
  return nullptr;
}

// statements
void Resolver::visitBlockStmt(BlockStmt& stmt) {
  beginScope();
  resolve(stmt.statements);
  endScope();

}

void Resolver::visitVarStmt(VarStmt& stmt) {
  declare(stmt.name);
  if (stmt.initializer != nullptr) {
    resolve(*stmt.initializer);
  }
  define(stmt.name);

}
