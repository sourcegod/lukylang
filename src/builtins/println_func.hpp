#ifndef PRINTLN_FUNC_HPP
#define PRINTLN_FUNC_HPP
#include "../lukobject.hpp"
#include <string>
#include <vector>
#include <iostream>

class LukObject;
class LukCallable;
class Interpreter;

class PrintlnFunc : public LukCallable {
public:
    PrintlnFunc() {}
    
    virtual size_t arity() override { return 0; }
    virtual ObjPtr  call(Interpreter& /*interp*/, 
           std::vector<ObjPtr>& v_args) override {
      if (v_args.size() >= 1)
        std::cout << v_args[0] << "\n";
      else
        std::cout << "\n";

        return nilptr;
   }
   
   virtual std::string toString() const override { return "<Native Function: println(...)>"; }

};

#endif // PRINTLN_FUNC_HPP
