#include "lukinstance.hpp"
#include "lukobject.hpp"
#include "lukclass.hpp"
#include "token.hpp"

#include <memory>

std::shared_ptr<LukObject> LukInstance::get(Token& tok) {
    auto elem = m_fields.find(tok.lexeme);
    if (elem != m_fields.end()) {
      return elem->second;
    }
  
    // return
}

std::ostream& operator<<(std::ostream& oss, const LukInstance& li) {
  oss << li.toString();

  return oss;
}

