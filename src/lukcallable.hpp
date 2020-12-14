#ifndef LUKCALLABLE_HPP
#define LUKCALLABLE_HPP

#include "common.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <sstream> // osstringstream
#include <memory> // smart pointers
#include <map>

class Interpreter;
using VArguments = std::vector<ObjPtr>;

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
    virtual ObjPtr call(Interpreter&, VArguments& v_args) =0;
    virtual std::string toString() const = 0;
    virtual std::string typeName() const { return "LukCallable"; }
    std::map<std::string, std::string>& getKeywords() { return m_keywords; }
    virtual void setKeywords(const std::string&, const std::string&) {}

protected:
   std::map<std::string, std::string> m_keywords;
};


#endif // LUKCALLABLE_HPP
