#ifndef LUKERROR_HPP
#define LUKERROR_HPP

#include <string>
#include <vector>
#include "token.hpp"


// namespace luky {
class LukError {
  public:
    LukError();
    void error(int line, int col, const std::string& message);
    void error(Token& token, std::string& message);

    void report(int line, int col, const std::string& where, 
            const std::string& message) const;
    bool hadError;

};

// }

#endif // LUKERROR_HPP
