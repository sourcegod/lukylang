#ifndef READLN_FUNC_HPP
#define READLN_FUNC_HPP
#include <string>
#include <vector>
#include <iostream>

class LukCallable;
class Interpreter;

class ReadlnFunc : public LukCallable {
public:
    ReadlnFunc() {}
    virtual size_t arity() override { return 1; }
    virtual ObjPtr  call(Interpreter& /*interp*/, 
           std::vector<ObjPtr>& v_args) override {
      if (v_args.size() >= 1) {

      } else {
        std::cout << "\n";
      }

      return nilptr;
   }
   
   virtual std::string toString() const override { return "<Native Function: println(...)>"; }

};

#endif // READLN_FUNC_HPP
