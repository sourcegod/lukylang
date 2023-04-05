#ifndef LUKCLASS_HPP
#define LUKCLASS_HPP

#include "common.hpp"
#include "lukcallable.hpp"
#include "lukinstance.hpp"
#include "logger.hpp"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace luky {
    class LukClass : public LukInstance, public LukCallable {
    public:
      std::string m_name;
      std::shared_ptr<LukClass> p_superclass;

        LukClass( std::shared_ptr<LukClass> metaclass,
              const std::string& name,
              std::shared_ptr<LukClass> superclass,
              const std::unordered_map<std::string, ObjPtr>& methods) :
          LukInstance(metaclass),
          m_name(name),
          p_superclass(superclass),
          m_methods(methods) 
        {}

        ~LukClass() {}

        virtual size_t arity() override;
        virtual std::string toString() const override;
        virtual ObjPtr  call(Interpreter& interp, std::vector<ObjPtr>& v_args) override;
        ObjPtr findMethod(const std::string& name);

    private:
      std::unordered_map<std::string, ObjPtr> m_methods;
    };
}

std::ostream& operator<<(std::ostream& oss, const luky::LukClass& lc);


#endif // LUKCLASS_HPP
