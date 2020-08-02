#include "lukclass.hpp"
#include "lukinstance.hpp"
#include "lukobject.hpp"
#include "lukfunction.hpp"
#include <memory>
#include <cassert>
#include <typeinfo>

size_t LukClass::arity() { 
    ObjPtr method = findMethod("init"); 
    if (method != nullptr) {
      std::shared_ptr<LukFunction> initializer = method->getDynCast<LukFunction>();
      if (initializer == nullptr) return 0;
      return initializer->arity();
    }

  
  return 0;
}

LukObject  LukClass::call(Interpreter& interp, 
           std::vector<LukObject>& v_args) {
    // Note: "this" is a const pointer, 
    // so the current function should be not const
    // otherwire "this" is casting const type* const
    // so not compile, when passing it as parameter to lukinstance object.
    auto thisPtr = std::make_shared<LukClass>(*this);
    auto instPtr = std::make_shared<LukInstance>(thisPtr );
    ObjPtr method = findMethod("init"); 
    if (method != nullptr) {
          // Note: to get derived object from shared_ptr base object
          // yout must use: static_pointer_cast or dynamic_pointer_cast to cast it.
          std::shared_ptr<LukFunction> initializer = method->getDynCast<LukFunction>();
          if (initializer != nullptr) {
            auto obj_ptr = initializer->bind(instPtr);
              obj_ptr->getCallable()->call(interp, v_args);
          }
    }

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

  if (p_superclass != nullptr) {
    return p_superclass->findMethod(name);
  }
  return nullptr;
}


std::ostream& operator<<(std::ostream& oss, const LukClass& lc) {
  oss << lc.m_name;

  return oss;
}

