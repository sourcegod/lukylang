#include "environment.hpp"
#include "token.hpp"
#include "runtimeerror.hpp"
#include <string>
#include <map>
#include <iostream>

using namespace luky;

// static variable must be initialized
int Environment::next_id;
ObjPtr& Environment::get(TokPtr& name) {
    auto iter = m_values.find(name->lexeme);
    if (iter != m_values.end()) {
        return iter->second;
    }
    
    if (m_enclosing != nullptr) {
        return m_enclosing->get(name);
    }

    throw RuntimeError(name, 
            "Undefined variable '" + name->lexeme + "'");
}

void Environment::assign(TokPtr& name, ObjPtr& val) {
    if ( m_values.find(name->lexeme) != m_values.end() ) {
        m_values[name->lexeme] = val; // std::make_shared<TObject>(val;
        return;
    }

    if (m_enclosing != nullptr) {
        m_enclosing->assign(name, val);
        return;
    }

    throw RuntimeError(name, 
            "Undefined variable '" + name->lexeme + "'");

}

void Environment::assign(TokPtr& name, std::shared_ptr<LukCallable> callable) {
  ObjPtr objP = std::make_shared<LukObject>(callable);
  assign(name, objP); // LukObject(callable));

}

void Environment::define(const std::string& name, ObjPtr val) {
    m_values[name] =  val;
}

ObjPtr Environment::getAt(int distance, const std::string& name) {
  auto& values = ancestor(distance)->m_values;
  auto iter = values.find(name);
  if (iter == values.end()) {
    std::ostringstream msg;
    msg << "Undefined variable '" << name << "' at distance: " << distance;
    throw RuntimeError(msg.str());
  }
  return iter->second;
}


Environment* Environment::ancestor(int distance) {
  auto env = this;
  for (int i=0; i< distance; i++) {
    if (env->m_enclosing != nullptr) 
      env = env->m_enclosing.get();
  }

  return env;
}

void Environment::assignAt(int distance, TokPtr& name, ObjPtr& val) {
  ancestor(distance)->m_values[name->lexeme] = val;

}
