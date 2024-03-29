#ifndef CLOCK_FUNC_HPP
#define CLOCK_FUNC_HPP
#include "../lukobject.hpp"
#include <string>
#include <chrono>
#include <vector>

namespace luky {
    class LukCallable;
    class Interpreter;

    class ClockFunc : public LukCallable {
    public:
        using TClock = std::chrono::high_resolution_clock;
        ClockFunc() { m_start = TClock::now(); }
        
        virtual size_t arity() override { return 0; }
        virtual ObjPtr  call(Interpreter& /*interp*/, 
               std::vector<ObjPtr>& /*args*/) override {
            double dur = std::chrono::duration<double>(TClock::now() - m_start).count();

            return std::make_shared<LukObject>(dur);
       }
       
       virtual std::string toString() const override { return "<Function clock()>"; }

    private:
       TClock::time_point m_start;
    };
}

#endif // CLOCK_FUNC_HPP
