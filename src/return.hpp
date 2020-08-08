#ifndef RETURN_HPP
#define RETURN_HPP
#include "lukobject.hpp"
#include <stdexcept> // exception

class Return : public std::exception {
public:
    explicit Return(ObjPtr& _value) 
        : value(_value) {} 
    
    ObjPtr value;
};

#endif //  RETURN_HPP
