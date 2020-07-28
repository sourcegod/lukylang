#include "environment.hpp"
#include "token.hpp"
#include "runtimeerror.hpp"
#include <string>
#include <map>
#include <iostream>

// static variable must be initialized
int Environment::next_id;
TObject& Environment::get(Token name) {
    auto elem = m_values.find(name.lexeme);
    if (elem != m_values.end()) {
        return *elem->second;
    }
    
    if (m_enclosing != nullptr) {
        return m_enclosing->get(name);
    }

    throw RuntimeError(name, 
            "Undefined variable '" + name.lexeme + "'");
}

void Environment::assign(Token name, TObject val) {
    if ( m_values.find(name.lexeme) != m_values.end() ) {
        m_values[name.lexeme] = std::make_shared<TObject>(val);
        return;
    }

    if (m_enclosing != nullptr) {
        m_enclosing->assign(name, val);
        return;
    }

    throw RuntimeError(name, 
            "Undefined variable '" + name.lexeme + "'");

}

void Environment::assign(Token name, std::shared_ptr<LukCallable> callable) {
  assign(name, LukObject(callable));

}

void Environment::define(const std::string& name, TObject val) {
    m_values[name] =  std::make_shared<TObject>(val);
}

TObject Environment::getAt(int distance, const std::string& name) {
  auto& values = ancestor(distance)->m_values;
  auto elem = values.find(name);
  if (elem == values.end()) {
    std::ostringstream msg;
    msg << "Undefined variable '" << name << "' at distance: " << distance
      << " at depth: ";
    throw RuntimeError(msg.str());
  }
  return *elem->second.get();
}


Environment* Environment::ancestor(int distance) {
  auto env = this;
  for (int i=0; i< distance; i++) {
    if (env->m_enclosing != nullptr) 
      env = env->m_enclosing.get();
  }

  return env;
}

void Environment::assignAt(int distance, Token& name, std::shared_ptr<TObject> val) {
  ancestor(distance)->m_values[name.lexeme] = val;

}
