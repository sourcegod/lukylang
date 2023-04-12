#ifndef JUMP_HPP
#define JUMP_HPP

#include "common.hpp"
#include <stdexcept> // runtime_error
#include <string>

namespace luky {
    class Jump : public std::runtime_error {
    public:
        Jump(TokPtr& keyword, const std::string& msg) 
            : std::runtime_error(keyword->lexeme + ", " + msg), 
            m_keyword(keyword) 
          {} 

    TokPtr m_keyword;
    };
}

#endif //  JUMP_HPP
