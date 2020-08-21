#include "lukfunction.hpp"
#include "lukobject.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "return.hpp"

ObjPtr  LukFunction::call(Interpreter& interp, std::vector<ObjPtr>& v_args) {
    TRACE_MSG("Call Function Tracer: ");
    // std::cerr << "interp.m_globals.size: " << interp.m_globals->size() << "\n";
    auto env = std::make_shared<Environment>(m_closure);
    for (unsigned i=0; i < m_declaration->params.size(); ++i) {
        // Note: C++ can store polymorphic or derived object in a container
        // only with pointer or smart pointers.
        env->define(m_declaration->params.at(i)->lexeme, v_args.at(i));
    }
    
    try {
        interp.executeBlock(m_declaration->body, env);
    } catch(Return& ret) {
        if (m_isInitializer) { 
          return m_closure->getAt(0, "this");
        }
        
        return ret.value;
    }
    if (m_isInitializer) return  m_closure->getAt(0, "this");
    
    
    return nilptr;
}

ObjPtr LukFunction::bind(std::shared_ptr<LukInstance> instPtr) {
  auto env = std::make_shared<Environment>(m_closure);
  ObjPtr objP = std::make_shared<LukObject>(instPtr);
  env->define("this", objP);
  auto funcPtr = std::make_shared<LukFunction>(m_declaration, env, m_isInitializer);
  auto obj_ptr = std::make_shared<LukObject>(funcPtr);
  return obj_ptr;
}

