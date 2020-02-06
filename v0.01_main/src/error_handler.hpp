#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>
#include <vector>
#include "token.hpp"


namespace luky {
    class ErrorHandler {
      public:
        ErrorHandler();
        void error(int line, int col, const std::string& message);
		void error(Token& token, std::string& message);

        void report(int line, int col, const std::string& where, 
				const std::string& message) const;
        bool hadError;

    };

}

#endif // ERROR_HANDLER_HPP
