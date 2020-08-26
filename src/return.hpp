#ifndef RETURN_HPP
#define RETURN_HPP
#include "common.hpp"
#include <stdexcept> // exception

class Return : public std::exception {
public:
    explicit Return(ObjPtr& value) 
        : m_value(value) 
    {} 
    
    ObjPtr m_value;
};

#endif //  RETURN_HPP
