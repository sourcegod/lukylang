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
      std::string line;
      if (v_args.size() >= 1) {
        std::cout << v_args[0];
      }
      
      while (1) {
        if (!getline(std::cin, line)) break;
        if (line.empty()) continue;
        else break;
      }

      return std::make_shared<LukObject>(line);
   }
   
   virtual std::string toString() const override { return "<Native Function: readln)arg)>"; }

};

#endif // READLN_FUNC_HPP
