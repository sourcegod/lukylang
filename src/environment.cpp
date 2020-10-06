#include "environment.hpp"
#include "token.hpp"
#include "runtimeerror.hpp"
#include <string>
#include <map>
#include <iostream>

TObject& Environment::get(Token name) {
    auto elem = values.find(name.lexeme);
    if (elem != values.end()) {
        return elem->second;
    }
    
    if (m_enclosing != nullptr) return m_enclosing->get(name);

    throw RuntimeError(name, 
            "Undefined variable '" + name.lexeme + "'");
}

void Environment::assign(Token name, TObject value) {
    if ( values.find(name.lexeme) != values.end() ) {
        values[name.lexeme]= value;
        return;
    }

    if (m_enclosing != nullptr) {
        m_enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, 
            "Undefined variable '" + name.lexeme + "'");

}

void Environment::define(std::string name, TObject value) {
    values[name] =  value;
}

