#ifndef LEN_FUNC_HPP
#define LEN_FUNC_HPP
#include <string>
#include <vector>

class LukCallable;
class Interpreter;

/// Note: retrieve string length
class LenFunc : public LukCallable {
public:
    LenFunc() {} 

    virtual size_t arity() override { return 1; }
    virtual ObjPtr  call(Interpreter& /*interp*/, 
           std::vector<ObjPtr>& v_args) override {
        if (v_args[0]->isString()) {
            int val = v_args[0]->toString().size();
            return std::make_shared<LukObject>(val);
        }
        
        return std::make_shared<LukObject>(0);
    }
   
    virtual std::string toString() const override { return "<Native Function: len()>"; }

};

#endif // LEN_FUNC_HPP
