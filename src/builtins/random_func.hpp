#ifndef RANDOM_FUNC_HPP
#define RANDOM_FUNC_HPP
#include <string>
#include <vector>
#include <cstdlib> // rand and srand

namespace luky {
    class LukCallable;
    class Interpreter;

    /// Note: convert any object to int
    class RandomFunc : public LukCallable {
    public:
        RandomFunc() {
            /// Note: initialize seed rand
            /// srand must be call once time
            std::srand(time(0));
        } 

        virtual size_t arity() override { 
            // take 0 or more parameters
            // // TODO: manage minimum parameters
            return 255; 
        }
        virtual ObjPtr  call(Interpreter& /*interp*/, 
               std::vector<ObjPtr>& v_args) override {
           
            auto size = v_args.size();
            TLukInt val =0;
            auto start = m_start;
            auto stop = m_stop;
            if (size == 1) stop = v_args[0]->toInt();
            else if (size == 2) {
                start = v_args[0]->toInt();
                stop = v_args[1]->toInt();
            }
            val = (rand() % stop);
            if (val < start) val += start;
          
            return std::make_shared<LukObject>(val);
        }
       
        virtual std::string toString() const override { return "<Native Function: random>"; }
    private:
        int m_start =0;
        int m_stop = 65536;

    };
}

#endif // RANDOM_FUNC_HPP
