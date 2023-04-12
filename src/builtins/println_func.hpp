#ifndef PRINTLN_FUNC_HPP
#define PRINTLN_FUNC_HPP
#include <string>
#include <vector>
#include <iostream>

namespace luky {
    class LukCallable;
    class Interpreter;

    class PrintlnFunc : public LukCallable {
    public:
        PrintlnFunc() { // : m_out(std::cout) {
            m_keywords["sep"] = " ";
            m_keywords["end"] = "\n";
            m_keywords["out"] = "stdout";
            m_out = &std::cout;

        }

        /// Note: 255 arguments means variadic function
        virtual size_t arity() override { return 255; }
        virtual ObjPtr  call(Interpreter& /*interp*/, 
               std::vector<ObjPtr>& v_args) override {
            if (isKeyworded) {
                /// Note: to switching to ostream out, you must make an ostream* pointer 
                /// to assign an ostream variable either to std::cout or std::cerr
                /// or simply: 
                /// std::ostream& out = condition ? std::cout : std::cerr;
                if (m_keywords["out"] == "stderr") m_out = &std::cerr;
                else m_out = &std::cout;
                isKeyworded = false;
            }
            
            if (v_args.size() >= 1) {
                for (size_t i=0; i<v_args.size(); i++) {
                    *m_out << v_args[i]; 
                    if (i < v_args.size() -1)
                        *m_out << m_keywords["sep"];
                }
                *m_out << m_keywords["end"];
            } else {
                *m_out << m_keywords["end"];
            }

            return nilptr;
        }
       
        virtual std::string toString() const override { return "<Native Function: println(...)>"; }
        virtual void setKeywords(const std::string& key, const std::string& value) override { 
            m_keywords[key] = value; 
            isKeyworded = true;
        }

    private:
        std::ostream* m_out;
        bool isKeyworded = false;

    };
}

#endif // PRINTLN_FUNC_HPP
