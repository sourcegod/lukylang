#include "lukfunction.hpp"
#include "lukobject.hpp"
#include "environment.hpp"
#include "interpreter.hpp"
#include "return.hpp"

LukObject  LukFunction::call(Interpreter& interp, std::vector<LukObject>& v_args) {
    TRACE_MSG("Call Function Tracer: ");
    // std::cerr << "interp.m_globals.size: " << interp.m_globals->size() << "\n";
    auto env = std::make_shared<Environment>(m_closure);
    for (unsigned i=0; i < m_declaration->params.size(); ++i) {
        // Note: C++ can store polymorphic or derived object in a container
        // only with pointer or smart pointers.
        env->define(m_declaration->params.at(i).lexeme, v_args.at(i));
    }
    
    try {
        interp.executeBlock(m_declaration->body, env);
    } catch(Return& ret) {
        return ret.value;
    }
    if (m_isInitializer) return  m_closure->getAt(0, "this");
    
    
    return TObject();
}

ObjPtr LukFunction::bind(std::shared_ptr<LukInstance> instPtr) {
  logMsg("\nIn LukFunction bind: ");
  logMsg("Instance: ", instPtr->toString());
  auto env = std::make_shared<Environment>(m_closure);
  logMsg("After define new environment, size: ", env->size());
  logMsg("Adding 'this' to env with: ", instPtr->toString());
  env->define("this", LukObject(instPtr));
  logMsg("After define 'this', env->size: ", env->size());
    logMsg("with instance: ", instPtr->toString());
  auto funcPtr = std::make_shared<LukFunction>(m_declaration, env, m_isInitializer);
  auto obj_ptr = std::make_shared<LukObject>(funcPtr);
  logMsg("Returns new lukfunction: ", obj_ptr->toString());
  logMsg("\nExit out LukFunction bind");
  return obj_ptr;
  // return TObject::getNilPtr();
}

