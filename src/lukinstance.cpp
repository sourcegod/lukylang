#include "lukinstance.hpp"
#include "lukclass.hpp"

#include <memory>


std::string LukInstance::toString() const { 
  return  "<" + m_klass->m_name + "  Instance>"; 
}

std::ostream& operator<<(std::ostream& oss, const LukInstance& li) {
  oss << li.toString();

  return oss;
}

