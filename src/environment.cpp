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

    throw RuntimeError(name, 
            "Undefined variable '" + name.lexeme + "'");

}

void Environment::assign(Token name, TObject value) {
    if ( values.find(name.lexeme) != values.end() ) {
        values[name.lexeme]= value;
        return;
    }

    throw RuntimeError(name, 
            "Undefined variable '" + name.lexeme + "'");

}

void Environment::define(std::string name, TObject value) {
    values[name] =  value;
}

