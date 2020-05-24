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
    auto elem = values.find(name.lexeme);
    logMsg("Env get: ", m_name);
    if (elem != values.end()) {
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
    if ( values.find(name.lexeme) != values.end() ) {
        values[name.lexeme] = std::make_shared<TObject>(val);
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
    values[name] =  std::make_shared<TObject>(val);
}

void Environment::define(const std::string& name, std::shared_ptr<TObject> val) {
  values[name] = val;
  logMsg("Env define: ", val->p_callable->addressOf());
  // logMsg("Env define: voici val->p_callable->toString: ", val->p_callable->toString());
  logMsg("Env define: voici val->p_callable->arity: ", val->p_callable->arity());
}
