#include "lukinstance.hpp"
#include "lukobject.hpp"
#include "lukclass.hpp"
#include "token.hpp"
#include "runtimeerror.hpp"

#include <memory>

std::string LukInstance::toString() const { 
  return  "<" + m_klass->m_name  + " instance>"; 
}

ObjPtr LukInstance::get(Token& name) {
    auto elem = m_fields.find(name.lexeme);
    if (elem != m_fields.end()) {
      return elem->second;
    }
  
    throw RuntimeError(name, 
        "Undefined property '" + name.lexeme + "'.");
    // unrichable
    return TObject::getNilPtr();
}
void LukInstance::set(Token& name, ObjPtr value) {
  m_fields[name.lexeme] = value;

}

std::ostream& operator<<(std::ostream& oss, const LukInstance& li) {
  oss << li.toString();

  return oss;
}

