#ifndef RUNTIMEERROR_HPP
#define RUNTIMEERROR_HPP

#include "token.hpp"
#include <stdexcept> // runtime_error
#include <string>

class RuntimeError : public std::runtime_error {
    
public:
    RuntimeError(const std::string& msg) 
        : std::runtime_error(msg) {}

    RuntimeError(Token& tok, const std::string& msg) 
        : std::runtime_error(tok.lexeme + ", " + msg) {}

};

#endif //  RUNTIMEERROR_HPP
