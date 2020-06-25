#include "lukclass.hpp"
#include "lukinstance.hpp"
#include "lukobject.hpp"

#include <memory>

size_t LukClass::arity() const { return 0; }
LukObject  LukClass::call(Interpreter& interp, 
           std::vector<LukObject>& v_args) {
    // Note: "this" is a const pointer, 
    // so the function should be not const
    // otherwire "this" is const type* const
    // so bug, when to pass it as parameter.
    auto instance = std::make_shared<LukInstance>( this );
    return LukObject(instance);
}

std::string LukClass::toString() const { return  "<Class " + m_name + ">"; }

std::ostream& operator<<(std::ostream& oss, const LukClass& lc) {
  oss << lc.m_name;

  return oss;
}

