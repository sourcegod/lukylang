#ifndef LUKFUNCTION_HPP
#define LUKFUNCTION_HPP

#include "environment.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
// #include "lukobject.hpp"

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
    LukFunction(FunctionStmt* declaration) { 
        m_declaration = declaration;
    
    }
    
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
        auto env = std::make_shared<Environment>(interp.m_globals);
        for (unsigned i=0; i < m_declaration->params.size(); ++i) {
            // Note: C++ can only store polymorphic or derived class
            // with pointer or smart pointers.
            env->define(m_declaration->params.at(i).lexeme, v_args.at(i));
        }

        interp.executeBlock(m_declaration->body, env);
        
        return LukObject();
   }
   

   virtual std::string toString() const override { return "<Function " + m_declaration->name.lexeme + ">"; }

private:
   FunctionStmt* m_declaration;

};

#endif // LUKFUNCTION_HPP
