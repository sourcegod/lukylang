#ifndef DOUBLE_FUNC_HPP
#define DOUBLE_FUNC_HPP

#include <string>
#include <vector>

class LukCallable;
class Interpreter;

class DoubleFunc : public LukCallable {
public:
    DoubleFunc() {} 

    virtual size_t arity() override { return 1; }
    virtual ObjPtr  call(Interpreter& /*interp*/, 
           std::vector<ObjPtr>& v_args) override {
       
           return std::make_shared<LukObject>(v_args[0]->toDouble());
    }
   
    virtual std::string toString() const override { return "<Native Function: double()>"; }

};

#endif // DOUBLE_FUNC_HPP
