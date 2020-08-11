#include "lukinstance.hpp"
#include "lukobject.hpp"
#include "lukclass.hpp"
#include "token.hpp"
#include "runtimeerror.hpp"
#include "lukfunction.hpp"
#include "environment.hpp"

#include <memory>

std::string LukInstance::toString() const { 
  // Note: FIX: m_klass can be null with default constructor
  // so, toString function must manage this case
  if (m_klass == nullptr) return "<Instance>";
  return  "<Instance " + m_klass->m_name  + ">"; 
}

ObjPtr LukInstance::get(TokPtr& name) {
    auto iter = m_fields.find(name->lexeme);
    if (iter != m_fields.end()) {
      return iter->second;
    }
    if (m_klass != nullptr) {
        ObjPtr method = m_klass->findMethod(name->lexeme); 
        // Note: to retrieve lukfunction,
        // you must extract lukfunction from lukobject
        if (method != nullptr && method->isCallable()) {
          // Note: to get derived object from shared_ptr base object
          // yout must use: static_pointer_cast or dynamic_pointer_cast to cast it.
          std::shared_ptr<LukFunction> funcPtr = method->getDynCast<LukFunction>();
          // must convert *this to lukinstance shared_ptr to be stored in a lukobject object.
          // Note: here deep copy is necessary cause, this object is more sophiscated object,
          // so the default copy constructor not suficient.
          std::shared_ptr<LukInstance> instPtr = std::make_shared<LukInstance>(*this);
          return funcPtr->bind(instPtr);
        }
    }

    throw RuntimeError(name, 
        "Undefined property '" + name.lexeme + "'.");
    // unrichable
    return nilptr;
}

void LukInstance::set(Token name, ObjPtr valPtr) {
  m_fields[name.lexeme] = valPtr;
  
}

std::ostream& operator<<(std::ostream& oss, const LukInstance& li) {
  oss << li.toString();

  return oss;
}

