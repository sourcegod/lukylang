#include "environment.hpp"
#include "token.hpp"
#include "runtimeerror.hpp"
#include <string>
#include <map>
#include <iostream>

// static variable must be initialized
int Environment::next_id;
TObject& Environment::get(Token name) {
    auto elem = values.find(name.lexeme);
    std::cerr << "Env get: " << m_name;
 
    
    if (elem != values.end()) {
        std::cerr << " var name: " << name.lexeme << ", " << *elem->second << "\n";
        
        return *elem->second;
    }
    
    if (m_enclosing != nullptr) {
        std::cerr << "\nEnv enclosing: " << m_enclosing->m_name << "\n";
        return m_enclosing->get(name);
    }

    throw RuntimeError(name, 
            "Undefined variable '" + name.lexeme + "'");
}

void Environment::assign(Token name, TObject value) {
    if ( values.find(name.lexeme) != values.end() ) {
        values[name.lexeme]= std::make_shared<TObject>(value);
        return;
    }

    if (m_enclosing != nullptr) {
        m_enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, 
            "Undefined variable '" + name.lexeme + "'");

}

void Environment::define(const std::string& name, TObject value) {
    values[name] =  std::make_shared<TObject>(value);
}

