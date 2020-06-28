#include "lukinstance.hpp"
#include "lukobject.hpp"
#include "lukclass.hpp"
#include "token.hpp"
#include "runtimeerror.hpp"

#include <memory>

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

std::ostream& operator<<(std::ostream& oss, const LukInstance& li) {
  oss << li.toString();

  return oss;
}

