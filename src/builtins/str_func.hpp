#ifndef STR_FUNC_HPP
#define STR_FUNC_HPP

#include <string>
#include <vector>
#include <sstream> // ostringstream

namespace luky {
    class LukCallable;
    class Interpreter;

    /// Note: convert any object to string
    class StrFunc : public LukCallable {
    public:
        StrFunc() {} 

        virtual size_t arity() override { return 255; }
        virtual ObjPtr  call(Interpreter& /*interp*/, 
               std::vector<ObjPtr>& v_args) override {
            std::ostringstream msg;
            for (auto& arg: v_args) {
                msg << arg->toString();
            }
       
            return std::make_shared<LukObject>(msg.str());
        }
       
        virtual std::string toString() const override { return "<Native Function: double()>"; }

    };
}

#endif // STR_FUNC_HPP
