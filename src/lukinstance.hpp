#ifndef LUKINSTANCE_HPP
#define LUKINSTANCE_HPP

#include <string>
#include <memory>
#include <unordered_map>

class LukClass;
class LukObject;
class Token;

class LukInstance  {
public:
    explicit LukInstance(LukClass* klass)
      : m_klass(klass)
    {}

    virtual std::string toString() const { return  "<m_klass->m_name  instance>"; }
    std::shared_ptr<LukObject> get(Token& tok);

private:
   LukClass* m_klass;
   std::unordered_map<std::string, std::shared_ptr<LukObject>> m_fields;

};

std::ostream& operator<<(std::ostream& oss, const LukInstance& li);

#endif // LUKINSTANCE_HPP
