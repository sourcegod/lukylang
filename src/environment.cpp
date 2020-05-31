#include "environment.hpp"
#include "token.hpp"
#include "runtimeerror.hpp"
#include <string>
#include <map>
#include <iostream>

// static variable must be initialized
int Environment::next_id;
TObject& Environment::get(Token name) {
  TRACE_MSG("Environment Get Tracer: ");
    auto elem = m_values.find(name.lexeme);
    logMsg("Env get: ", m_name);
    if (elem != m_values.end()) {
        logMsg("var name: ", name.lexeme, ",", *elem->second);
        if (elem->second->isCallable()) {
          // logMsg("elem is callable, ", "bUG when calling function in the REPL");
          logMsg("Env get: ", elem->second->p_callable->addressOf());
          logMsg(elem->second->p_callable);
          logMsg("Env get, Arity: ", elem->second->p_callable->arity());
        }
        
        return *elem->second;
    }
    
    if (m_enclosing != nullptr) {
        logMsg("\nEnv enclosing: ", m_enclosing->m_name);
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

void Environment::define(const std::string& name, TObject val) {
    m_values[name] =  std::make_shared<TObject>(val);
}

void Environment::define(const std::string& name, std::shared_ptr<TObject> val) {
  m_values[name] = val;
  logMsg("Env define: ", val->p_callable->addressOf());
  // logMsg("Env define: voici val->p_callable->toString: ", val->p_callable->toString());
  logMsg("Env define: voici val->p_callable->arity: ", val->p_callable->arity());
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
  
  return elem->second.get();
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
