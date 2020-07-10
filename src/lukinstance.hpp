#ifndef LUKINSTANCE_HPP
#define LUKINSTANCE_HPP

#include "lukobject.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include "lukclass.hpp"
class LukClass;
class LukObject;
class Token;
using ObjPtr = std::shared_ptr<LukObject>;

class LukInstance  {
// FIXME: temporary, this alias is allready in lukobject.hpp
public:
   std::unordered_map<std::string, int> m_map;
    LukInstance(const LukInstance&) {
      m_klass = nullptr;
    }
    explicit LukInstance(LukClass* klass)
      : m_klass(klass) { 
        std::cerr << "LukInstance in constructor\n";
        std::cerr << "m_klass->tostring: " << m_klass->toString() << "\n";
        std::cerr << "Exit out constructor\n";
      }

    virtual std::string toString() const;
    ObjPtr get(Token& name);
    void set(Token name, ObjPtr valPtr);

   LukClass* m_klass;
   std::unordered_map<std::string, ObjPtr> m_fields = {};
private:

};

std::ostream& operator<<(std::ostream& oss, const LukInstance& li);

#endif // LUKINSTANCE_HPP
