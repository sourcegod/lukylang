#include "lukerror.hpp"
#include <iostream>

LukError::LukError()
    : hadError(false) {}

void LukError::error(int line, int col,
                       const std::string& message) {
    report(errTitle, line, col, "", message);
    hadError = true;
}

void LukError::error(const std::string& title, int line, int col,
                       const std::string& message) {
	report(title, line, col, "", message);
    hadError = true;
}


void LukError::error(const std::string& title, Token& token, 
        const std::string& message) {
	if (token.type == TokenType::END_OF_FILE) {
      report(title, token.line, token.col, " at end", message);
    } else {
      report(title, token.line, token.col, " at '" + token.lexeme + "'", 
              message);
    }

    hadError = true;
}

void LukError::report(const std::string& title, int line, int col, 
        const std::string& where, 
        const std::string& message) const {
	std::cerr << title << "[line " + std::to_string(line) + 
        " col " + std::to_string(col) + "]" + 
		where + ": " + message
		<< std::endl;
    
}

