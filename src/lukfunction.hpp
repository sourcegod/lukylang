#ifndef LUKFUNCTION_HPP
#define LUKFUNCTION_HPP

#include "environment.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
#include "lukobject.hpp"
#include "return.hpp"
#include "logger.hpp"

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <typeinfo> // type name

class Stmt;
class FunctionStmt;
class LukObject;
class LukCallable;
class Environment;
class Interpreter;

class LukFunction : public LukCallable {
public:
    // WARNING: cannot copy assignment derived object like FunctionStmt ..
    // so passing it by pointer.
    LukFunction(FunctionStmt* declaration, PEnvironment closure) : 
      m_declaration(declaration),
      m_closure(closure) {}
    
    LukFunction() = delete;
    LukFunction(const LukFunction&) = delete;
    LukFunction(LukFunction&&) = delete;

    ~LukFunction() {} 
    virtual std::string addressOf()  override {  
        std::ostringstream oss;
        oss << "LukFunction address: " << this;
        return oss.str();
    }
    
    virtual size_t arity() const override { return m_declaration->params.size(); }
    virtual LukObject  call(Interpreter& interp, 
           std::vector<LukObject>& v_args) const override {

        TRACE_MSG("Call Function Tracer: ");
        // std::cerr << "interp.m_globals.size: " << interp.m_globals->size() << "\n";
        auto env = std::make_shared<Environment>(m_closure);
        for (unsigned i=0; i < m_declaration->params.size(); ++i) {
            // Note: C++ can store polymorphic or derived object in a container
            // only with pointer or smart pointers.
            env->define(m_declaration->params.at(i).lexeme, v_args.at(i));
        }
        
        try {
            logMsg("LukFunction, env name: ", env->m_name, "size: ", env->size(), 
                "\nm_closure name: ", m_closure->m_name, "size: ", m_closure->size());
            interp.executeBlock(m_declaration->body, env);
        } catch(Return& ret) {
            logMsg("Catch Return Value: ", ret.value.value());
            return ret.value;
        }
        
        return LukObject();
   }
   

   virtual std::string toString() const override { return "<Function " + m_declaration->name.lexeme + ">"; }

private:
   FunctionStmt* m_declaration;
   PEnvironment m_closure;

};

#endif // LUKFUNCTION_HPP
