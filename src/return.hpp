#ifndef RETURN_HPP
#define RETURN_HPP
#include "lukobject.hpp"
#include <stdexcept> // exception

class Return : public std::exception {
public:
    explicit Return(TObject& _value) 
        : value(_value) {} 
    
    TObject value;
};

#endif //  RETURN_HPP
