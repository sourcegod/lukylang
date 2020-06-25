#ifndef LUKINSTANCE_HPP
#define LUKINSTANCE_HPP

// #include "lukobject.hpp"
#include "lukclass.hpp"

#include <string>
#include <vector>
#include <memory>


class LukInstance  {
public:
    explicit LukInstance(LukClass* klass)
    : m_klass(klass)
    {}

   virtual std::string toString() const; // { return  "<m_klass->m_name  instance>"; }

private:
   LukClass* m_klass;

};

std::ostream& operator<<(std::ostream& oss, const LukInstance& li);

#endif // LUKINSTANCE_HPP
