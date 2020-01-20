#ifndef LUKFUNCTION_HPP
#define LUKFUNCTION_HPP

#include "environment.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
// #include "lukobject.hpp"

#include <string>
#include <vector>
#include <memory>
#include "return.hpp"
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

        // std::cerr << "interp.m_globals.size: " << interp.m_globals->size() << "\n";
        auto env = std::make_shared<Environment>(interp.m_globals);
        // auto env = std::make_shared<Environment>();
        // std::cerr << "env.size: " << env->size() << "\n";
        for (unsigned i=0; i < m_declaration->params.size(); ++i) {
            // Note: C++ can store polymorphic or derived object in a container
            // only with pointer or smart pointers.
            env->define(m_declaration->params.at(i).lexeme, v_args.at(i));
        }
        
        try {
            // std::cout << "in lukfunction\n";
            // std::cerr << "env.size: " << env->size() << "\n";
            // std::cerr << "interp.m_globals.size: " << interp.m_globals->size() << "\n";
            interp.executeBlock(m_declaration->body, env);
            // std::cout << "after body\n";
        } catch(Return& ret) {
            // std::cerr << "je return: " << ret.value.value() << "\n";
            return ret.value;
        }
        
        return LukObject();
   }
   

   virtual std::string toString() const override { return "<Function " + m_declaration->name.lexeme + ">"; }

private:
   FunctionStmt* m_declaration;

};

#endif // LUKFUNCTION_HPP
