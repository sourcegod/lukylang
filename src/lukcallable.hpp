#ifndef LUKCALLABLE_HPP
#define LUKCALLABLE_HPP

// #include "lukobject.hpp"
// #include "stmt.hpp"
// #include "interpreter.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <sstream> // osstringstream
#include <memory> // smart pointers

class LukObject;
class Interpreter;
using VArguments = std::vector<LukObject>;

class LukCallable {
public:
    LukCallable() {}
    ~LukCallable() {}
    
    virtual std::string addressOf() {  
        std::ostringstream oss;
        oss << "LukCallable address: " << this;
        return oss.str();
    }
 
    virtual size_t arity() = 0;
    virtual std::shared_ptr<LukObject> call(Interpreter&, VArguments& v_args) =0;
    virtual std::string toString() const = 0;
   virtual std::string typeName() const { return "LukCallable"; }
};


#endif // LUKCALLABLE_HPP
