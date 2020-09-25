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
    /// Note: 255 arguments means variadic function
    virtual size_t arity() override { return 255; }
    virtual ObjPtr  call(Interpreter& /*interp*/, 
           std::vector<ObjPtr>& v_args) override {
      if (v_args.size() >= 1) {
        for (auto& it: v_args) {
          std::cout << it << " ";
        }
        std::cout << "\n";

      } else {
        std::cout << "\n";
      }

      return nilptr;
   }
   
   virtual std::string toString() const override { return "<Native Function: println(...)>"; }

};

#endif // PRINTLN_FUNC_HPP
