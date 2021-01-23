#ifndef TYPE_FUNC_HPP
#define TYPE_FUNC_HPP
#include <string>
#include <vector>

class LukCallable;
class Interpreter;

/// Note: returns the string representation for any object
class TypeFunc : public LukCallable {
public:
    TypeFunc() {} 

    virtual size_t arity() override { return 1; }
    virtual ObjPtr  call(Interpreter& /*interp*/, 
           std::vector<ObjPtr>& v_args) override {
       
        return std::make_shared<LukObject>(v_args[0]->typeOf());
    }
   
    virtual std::string toString() const override { return "<Native Function: type()>"; }

};

#endif // TYPE_FUNC_HPP
