#ifndef JUMP_HPP
#define JUMP_HPP

#include "token.hpp"
#include <stdexcept> // runtime_error
#include <string>

class Jump : public std::runtime_error {
    
public:
    Jump(Token& key, const std::string& msg) 
        : std::runtime_error(key.lexeme + ", " + msg), 
        keyword(key) {} 
    Token keyword;
};

#endif //  JUMP_HPP
