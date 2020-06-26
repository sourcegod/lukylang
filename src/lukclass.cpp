#include "lukclass.hpp"
#include "lukinstance.hpp"
#include "lukobject.hpp"

#include <memory>

size_t LukClass::arity() const { return 0; }
LukObject  LukClass::call(Interpreter& interp, 
           std::vector<LukObject>& v_args) {
    // Note: "this" is a const pointer, 
    // so the current function should be not const
    // otherwire "this" is casting const type* const
    // so not compile, when passing it as parameter to lukinstance object.
    auto instance = std::make_shared<LukInstance>( this );
    return LukObject(instance);
}

std::string LukClass::toString() const { return  "<Class " + m_name + ">"; }

std::ostream& operator<<(std::ostream& oss, const LukClass& lc) {
  oss << lc.m_name;

  return oss;
}

