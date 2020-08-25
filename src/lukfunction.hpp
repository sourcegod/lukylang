#ifndef LUKFUNCTION_HPP
#define LUKFUNCTION_HPP

#include "common.hpp"
#include "lukcallable.hpp"
#include "expr.hpp"
#include "logger.hpp"

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <typeinfo> // type name

class LukFunction : public LukCallable {
public:
    // Note: WARNING: cannot copy assignment derived object like FunctionStmt ..
    // so passing it by raw pointer.
    LukFunction() {}    
    LukFunction(const std::string& name, std::shared_ptr<FunctionExpr>& declaration, 
        EnvPtr closure,
        bool isInitializer) : 
      m_name(name),
      m_declaration(declaration),
      m_closure(closure), 
      m_isInitializer(isInitializer) {
    }

    ~LukFunction() { 
    } 
    
    virtual std::string addressOf()  override {  
        std::ostringstream oss;
        oss << "LukFunction address: " << this;
        return oss.str();
    }
    virtual std::string typeName() const override { return "LukFunction"; }
    
    virtual size_t arity() override { return m_declaration->m_params.size(); }
    virtual ObjPtr  call(Interpreter& interp, std::vector<ObjPtr>& v_args) override;
    virtual std::string toString() const override { 
      if (m_name == "") return "<Function Lambda>";
      return "<Function " + m_name + ">"; 
    }
    ObjPtr bind(std::shared_ptr<LukInstance> instPtr);

private:
    const std::string m_name;
    std::shared_ptr<FunctionExpr> m_declaration;
    EnvPtr m_closure;
    bool m_isInitializer;

};

#endif // LUKFUNCTION_HPP
