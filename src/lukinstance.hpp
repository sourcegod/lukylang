#ifndef LUKINSTANCE_HPP
#define LUKINSTANCE_HPP

#include "lukobject.hpp"
#include "lukclass.hpp"
#include "logger.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>

class LukClass;
class LukObject;
class Token;
using ObjPtr = std::shared_ptr<LukObject>;

class LukInstance {
// FIXME: temporary, this alias is allready in lukobject.hpp
public:
    explicit LukInstance(std::shared_ptr<LukClass> klass)
      : m_klass(klass) { 
    }
      
    explicit LukInstance(LukInstance& other) {
      // Note: we should do a deep copy for this object
      // cause this object is more sophisticated
      // so the compiler default copy constructor cannot copy it entirely.
      logMsg("LukInstance copy Ctor with deep copy");
      m_klass = other.getKlass();
      m_fields = other.getFields();
    }
    
    ~LukInstance() { 
    }

      std::shared_ptr<LukInstance> clone() {
        return std::make_shared<LukInstance>(*this);
      }

      std::shared_ptr<LukClass>& getKlass() { return m_klass; }
      std::unordered_map<std::string, ObjPtr>& getFields() { return m_fields; }

    virtual std::string toString() const;
    ObjPtr get(Token& name);
    void set(Token name, ObjPtr valPtr);

private:
   // LukClass* m_klass;
    std::shared_ptr<LukClass> m_klass;
   std::unordered_map<std::string, ObjPtr> m_fields = {};

};

std::ostream& operator<<(std::ostream& oss, const LukInstance& li);

#endif // LUKINSTANCE_HPP
