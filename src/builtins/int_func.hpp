#ifndef INT_FUNC_HPP
#define INT_FUNC_HPP
#include <string>
#include <vector>
#include <iostream>

class LukCallable;
class Interpreter;

class IntFunc : public LukCallable {
public:
    IntFunc() {} 

    virtual size_t arity() override { return 1; }
    virtual ObjPtr  call(Interpreter& /*interp*/, 
           std::vector<ObjPtr>& v_args) override {
       
           return std::make_shared<LukObject>(v_args[0]->toInt());
    }
   
    virtual std::string toString() const override { return "<Native Function: int()>"; }

};

#endif // INT_FUNC_HPP
