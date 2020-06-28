#ifndef LUKINSTANCE_HPP
#define LUKINSTANCE_HPP

#include "lukobject.hpp"
#include <string>
#include <memory>
#include <unordered_map>

class LukClass;
class LukObject;
class Token;

class LukInstance  {
  // FIXME: temporary, this alias is allready in lukobject.hpp
    using ObjPtr = std::shared_ptr<LukObject>;
public:
    explicit LukInstance(LukClass* klass)
      : m_klass(klass)
    {}

    virtual std::string toString() const { return  "<m_klass->m_name  instance>"; }
    ObjPtr get(Token& name);
    void set(Token& name, ObjPtr value);

private:
   LukClass* m_klass;
   std::unordered_map<std::string, ObjPtr> m_fields;

};

std::ostream& operator<<(std::ostream& oss, const LukInstance& li);

#endif // LUKINSTANCE_HPP
