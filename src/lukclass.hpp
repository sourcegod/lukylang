#ifndef LUKCLASS_HPP
#define LUKCLASS_HPP

#include "environment.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
#include "lukobject.hpp"
#include "return.hpp"
#include "logger.hpp"

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <typeinfo> // type name

class Stmt;
class FunctionStmt;
class LukObject;
class LukCallable;
class Environment;
class Interpreter;

class LukClass : public LukCallable {
public:
   std::string m_name;
    LukClass(const std::string& name)
    : m_name(name)
    {}

    virtual size_t arity() const override { return 0; }
    virtual LukObject  call(Interpreter& interp, 
           std::vector<LukObject>& v_args) const override {
       
        return TObject();
   }

   virtual std::string toString() const override { return  "<Class " + m_name + ">"; }

private:

};

std::ostream& operator<<(std::ostream& oss, const LukClass& lc) {
  oss << lc.m_name;

  return oss;
}

#endif // LUKCLASS_HPP
