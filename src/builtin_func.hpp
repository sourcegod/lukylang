#ifndef BUILTIN_FUNC_HPP
#define BUILTIN_FUNC_HPP

#include "common.hpp"
#include "builtins/clock_func.hpp"
#include "builtins/double_func.hpp"
#include "builtins/int_func.hpp"
#include "builtins/println_func.hpp"
#include "builtins/random_func.hpp"
#include "builtins/readln_func.hpp"
#include "builtins/str_func.hpp"
#include "builtins/type_func.hpp"
#include "builtins/len_func.hpp"

class BuiltinFunc {
public:
    BuiltinFunc(EnvPtr env) : m_env(env)
    {}
    
    void initNative() {
        // native clock function
        auto clock_func = std::make_shared<ClockFunc>();
        m_env->define("clock", std::make_shared<LukObject>(clock_func));
 
        // native double function
        auto double_func = std::make_shared<DoubleFunc>();
        m_env->define("double", std::make_shared<LukObject>(double_func));

        // native int function
        auto int_func = std::make_shared<IntFunc>();
        m_env->define("int", std::make_shared<LukObject>(int_func));

        // native println function
        auto println_func = std::make_shared<PrintlnFunc>();
        m_env->define("println", std::make_shared<LukObject>(println_func));
        
        // native random function
        auto random_func = std::make_shared<RandomFunc>();
        m_env->define("random", std::make_shared<LukObject>(random_func));
     
        // native readln function
        auto readln_func = std::make_shared<ReadlnFunc>();
        m_env->define("readln", std::make_shared<LukObject>(readln_func));
        
        // native str function
        auto str_func = std::make_shared<StrFunc>();
        m_env->define("str", std::make_shared<LukObject>(str_func));

        
        // native type function
        auto type_func = std::make_shared<TypeFunc>();
        m_env->define("type", std::make_shared<LukObject>(type_func));

        // native len function
        auto len_func = std::make_shared<LenFunc>();
        m_env->define("len", std::make_shared<LukObject>(len_func));


}

private:
    EnvPtr m_env;
};

#endif // BUILTIN_FUNC_HPP
