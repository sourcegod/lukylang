#include "lukfunction.hpp"
#include "lukobject.hpp"
#include "lukcallable.hpp"
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
    
    return TObject();
}

