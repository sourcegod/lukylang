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

ObjPtr LukInstance::get(Token& name) {
    logMsg("\nIn lukinstance get: ");
    logMsg("m_fields size: ", m_fields.size());
    logMsg("search name: ", name.lexeme);
    auto elem = m_fields.find(name.lexeme);
    logMsg("After m_fields size: ", m_fields.size());
    if (elem != m_fields.end()) {
      logMsg("Find elem in m_fields: ", elem->second);
      return elem->second;
    }
    logMsg("Not find elem in m_fields for name: ", name);
    logMsg("Searching for method, with  findMethod");
    if (m_klass != nullptr) {
      logMsg("m_klass is not null: ", m_klass->toString());
        ObjPtr method = m_klass->findMethod(name.lexeme); 
        logMsg("After findMethod: ", *method);
        // Note: to retrieve lukfunction,
        // you must extract lukfunction from lukobject
        if (method != nullptr && method->isCallable()) {
          // Note: to get derived object from shared_ptr base object
          // yout must use: static_pointer_cast or dynamic_pointer_cast to cast it.
          logMsg("Before dynamic_cast : ");
          std::shared_ptr<LukFunction> funcPtr = std::dynamic_pointer_cast<LukFunction>(method->getCallable());
          // std::shared_ptr<LukFunction> funcPtr(method->getCallable());
          std::shared_ptr<LukCallable> callable = method->getCallable();
          logMsg("callable typeName: ", callable->typeName());
          logMsg("funcPtr toString: ", funcPtr->toString());
          logMsg("funcPtr typeName: ", funcPtr->typeName());
          // must convert *this to lukinstance shared_ptr to be stored in a lukobject object.
          logMsg("After dynamic_cast : ");
          logMsg("create new instance: ");
          logMsg("this toString: ", this->toString());
          logMsg("this m_fields size: ", this->m_fields.size());
          // Note: diff ways to convert raw pointer to shared_ptr pointer
          // Note: here deep copy is necessary cause, this object is more sophiscated object,
          // so the default copy constructor not suficient.
          std::shared_ptr<LukInstance> instPtr = std::make_shared<LukInstance>(*this);
          // std::shared_ptr<LukInstance> instPtr(this);
          logMsg("instPtr use_count: ", instPtr.use_count());
          // Note: temporary create a new pointer count
          // to avoid double free corruption when delete lukinstance object
          // p_instance = instPtr;
          // logMsg("p_instance use_count: ", p_instance.use_count());
          logMsg("instPtr tostring: ", instPtr->toString());
          logMsg("instPtr m_fields size: ", instPtr->m_fields.size());
          return funcPtr->bind(instPtr);
        }
        
        logMsg("No findMethod");
    }
    logMsg("m_klass is null: ", "");
    throw RuntimeError(name, 
        "Undefined property '" + name.lexeme + "'.");
    // unrichable
    return TObject::getNilPtr();
}

void LukInstance::set(Token name, ObjPtr valPtr) {
  logMsg("\nIn LukInstance set, name: ", name.lexeme, ", value: ", *valPtr);
  logMsg("m_fields size: ", m_fields.size());
  logMsg("Adding to m_fields, name: ", name.lexeme,", value: ", *valPtr 
   ,", in m_klass: ", m_klass->toString());
  m_fields[name.lexeme] = valPtr;
  logMsg("m_fields size: ", m_fields.size());
  
  logMsg("Exit out lukinstance Set");
}

std::ostream& operator<<(std::ostream& oss, const LukInstance& li) {
  oss << li.toString();

  return oss;
}

