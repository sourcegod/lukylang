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
    LukFunction(FunctionStmt* declaration, std::shared_ptr<Environment> closure) : 
      m_declaration(declaration),
      m_closure(closure) {}
    

    ~LukFunction() { std::cerr << "Destructor lukfunction: " << this << "\n"; } 
    virtual std::string addressOf()  override {  
        std::ostringstream oss;
        oss << "LukFunction address: " << this;
        return oss.str();
    }
    
    virtual size_t arity() const override { return m_declaration->params.size(); }
    virtual LukObject  call(Interpreter& interp, std::vector<LukObject>& v_args) override;
   virtual std::string toString() const override { return "<Function " + m_declaration->name.lexeme + ">"; }

private:
   FunctionStmt* m_declaration;
   std::shared_ptr<Environment> m_closure;

};

#endif // LUKFUNCTION_HPP
