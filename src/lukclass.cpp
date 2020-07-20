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
      std::shared_ptr<LukFunction> initializer = std::dynamic_pointer_cast<LukFunction>(method->getCallable());
      if (initializer == nullptr) return 0;
      return initializer->arity();
    }

  
  return 0;
}
LukObject  LukClass::call(Interpreter& interp, 
           std::vector<LukObject>& v_args) {
  logMsg("\nIn call, LukClass");
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
    ObjPtr method = findMethod("init"); 
    if (method != nullptr) {

          // Note: to get derived object from shared_ptr base object
          // yout must use: static_pointer_cast or dynamic_pointer_cast to cast it.
          auto callable = method->getCallable();
          // auto funcPtr = std::make_shared<LukFunction>(callable);
          std::shared_ptr<LukFunction> initializer = std::dynamic_pointer_cast<LukFunction>(method->getCallable());
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
  
  return nullptr;
}


std::ostream& operator<<(std::ostream& oss, const LukClass& lc) {
  oss << lc.m_name;

  return oss;
}

