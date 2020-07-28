#ifndef LUKCLASS_HPP
#define LUKCLASS_HPP

#include "lukcallable.hpp"
#include "logger.hpp"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using ObjPtr = std::shared_ptr<LukObject>;
class LukClass : public LukCallable {
public:
    std::string m_name;

    LukClass(const std::string& name, 
        const std::unordered_map<std::string, ObjPtr>& methods)
      : m_name(name), 
    m_methods(methods) 
    {
    }

    ~LukClass() {
    }

    virtual size_t arity() override;
    virtual LukObject  call(Interpreter& interp, std::vector<LukObject>& v_args) override;
    virtual std::string toString() const override;
    ObjPtr findMethod(const std::string& name);

private:
  std::unordered_map<std::string, ObjPtr> m_methods;
};

std::ostream& operator<<(std::ostream& oss, const LukClass& lc);


#endif // LUKCLASS_HPP
