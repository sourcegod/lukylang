#ifndef LUKERROR_HPP
#define LUKERROR_HPP

#include "common.hpp"
#include "token.hpp"
#include <string>
#include <vector>
namespace luky {
    class LukError {
    public:
        bool hadError;

        LukError();
        void error(int line, int col, const std::string& message);
        void error(const std::string& title, int line, int col, 
                const std::string& message);
        void error(const std::string& title, TokPtr& tok, const std::string& message);
        void error(const std::string& title, const std::string& message);
        void report(const std::string& title, int line, int col, 
                const std::string& where, 
                const std::string& message) const;

    private:
      const std::string errTitle = "LukError: ";


    };
}

#endif // LUKERROR_HPP
