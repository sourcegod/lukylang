#ifndef LUKCLASS_HPP
#define LUKCLASS_HPP

#include "lukcallable.hpp"

#include <string>
#include <vector>
#include <memory>


class LukClass : public LukCallable {
public:
   std::string m_name;
    LukClass(const std::string& name)
    : m_name(name) {}

    virtual size_t arity() const override;
    virtual LukObject  call(Interpreter& interp, 
           std::vector<LukObject>& v_args) override;

   virtual std::string toString() const override;

private:

};

std::ostream& operator<<(std::ostream& oss, const LukClass& lc);


#endif // LUKCLASS_HPP
