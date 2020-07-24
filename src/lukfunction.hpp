#ifndef LUKFUNCTION_HPP
#define LUKFUNCTION_HPP

#include "lukcallable.hpp"
#include "stmt.hpp"
#include "logger.hpp"

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <typeinfo> // type name

class LukObject;
class Environment;
class Interpreter;

class LukFunction : public LukCallable {
public:
    // Note: WARNING: cannot copy assignment derived object like FunctionStmt ..
    // so passing it by raw pointer.
    LukFunction() {}    
    // LukFunction(FunctionStmt* declaration, std::shared_ptr<Environment> closure,
    LukFunction(std::shared_ptr<FunctionStmt>& declaration, std::shared_ptr<Environment> closure,
        bool isInitializer) : 
      m_declaration(declaration),
      m_closure(closure), 
  m_isInitializer(isInitializer) {
        logMsg("LukFunction constructor: ", this->toString());
      }

    ~LukFunction() { 
        logMsg("~Lukfunction destructor: ", this->toString()); 
    } 
    
    virtual std::string addressOf()  override {  
        std::ostringstream oss;
        oss << "LukFunction address: " << this;
        return oss.str();
    }
    virtual std::string typeName() const override { return "LukFunction"; }
    
    virtual size_t arity() override { return m_declaration->params.size(); }
    virtual LukObject  call(Interpreter& interp, std::vector<LukObject>& v_args) override;
    virtual std::string toString() const override { return "<Function " + m_declaration->name.lexeme + ">"; }
    ObjPtr bind(std::shared_ptr<LukInstance> instPtr);

private:
   // FunctionStmt* m_declaration;
    std::shared_ptr<FunctionStmt> m_declaration;
   std::shared_ptr<Environment> m_closure;
   bool m_isInitializer;

};

#endif // LUKFUNCTION_HPP
