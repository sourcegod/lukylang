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
  return  "<" + m_klass->m_name  + " instance>"; 
}

ObjPtr LukInstance::get(Token& name) {
    std::cerr << "\nIn lukinstance get: \n";
    auto elem = m_fields.find(name.lexeme);
    if (elem != m_fields.end()) {
      return elem->second;
    }
    ObjPtr method = m_klass->findMethod(name.lexeme);  
    // Note: to retrieve lukfunction,
    // you must extract lukfunction from lukobject
    if (method != nullptr && method->isCallable()) {
      // Note: to get derived object from shared_ptr base object
      // yout must use: static_pointer_cast or dynamic_pointer_cast to cast it.
      std::cerr << "Before dynamic_cast : \n";
      std::shared_ptr<LukFunction> funcPtr = std::dynamic_pointer_cast<LukFunction>(method->getCallable());
      std::shared_ptr<LukCallable> callable = method->getCallable();
      std::cerr << "callable typeName: " << callable->typeName() << "\n";
      std::cerr << "funcPtr toString: " << funcPtr->toString() << "\n";
      std::cerr << "funcPtr typeName: " << funcPtr->typeName() << "\n";
      // must convert *this to lukinstance shared_ptr to be stored in a lukobject object.
      std::cerr << "After dynamic_cast : \n";
      auto instPtr = std::make_shared<LukInstance>(*this);
      return funcPtr->bind(instPtr);
    }
    throw RuntimeError(name, 
        "Undefined property '" + name.lexeme + "'.");
    // unrichable
    return TObject::getNilPtr();
}
void LukInstance::set(Token name, ObjPtr valPtr) {
  std::cerr << "LukInstance in set, name: " << name.lexeme << ", value: " << *valPtr << "\n";
  m_fields[name.lexeme] = valPtr;
  std::cerr << "m_klass->tostring: " << m_klass->toString() << "\n";
  std::cerr << "voici toto: " << m_map["toto"] << "\n";
  // m_fields[] = std::make_shared<LukObject>(5);
  
  std::cerr << "Exit out lukinstance Set\n";
}

std::ostream& operator<<(std::ostream& oss, const LukInstance& li) {
  oss << li.toString();

  return oss;
}

