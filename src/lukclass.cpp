#include "lukclass.hpp"
#include "lukinstance.hpp"
#include "lukobject.hpp"

#include <memory>
#include <cassert>
#include <typeinfo>

size_t LukClass::arity() const { return 0; }
LukObject  LukClass::call(Interpreter& interp, 
           std::vector<LukObject>& v_args) {
    logMsg("LukClass in call");
    // Note: "this" is a const pointer, 
    // so the current function should be not const
    // otherwire "this" is casting const type* const
    // so not compile, when passing it as parameter to lukinstance object.
    auto thisPtr = std::make_shared<LukClass>(*this);
    auto instPtr = std::make_shared<LukInstance>(thisPtr );
    // assert(typeid(*this) == typeid(*instPtr));
    logType("typeid(*this): ", *this);
    logMsg("typeid(*instPtr): ", typeid(*instPtr).name());
    logMsg("after instance: m_klass toString: ", instPtr->getKlass()->toString());
    logMsg("instPtr->m_fields.size: ", instPtr->getFields().size());
    logMsg("Exit out LukClass\n");
    return LukObject(instPtr);
}

std::string LukClass::toString() const {  
  return  "<Class " + m_name + ">";
}

ObjPtr LukClass::findMethod(const std::string& name) {
  auto elem = m_methods.find(name);
  if (elem != m_methods.end()) {
    return elem->second;
  }
  
  return nullptr;
}


std::ostream& operator<<(std::ostream& oss, const LukClass& lc) {
  oss << lc.m_name;

  return oss;
}

