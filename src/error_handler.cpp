#include "error_handler.hpp"
#include <iostream>

using namespace luky;

ErrorHandler::ErrorHandler()
    : hadError(false) {}

void ErrorHandler::error(int line, int col,
                       const std::string& message) {
	report(line, col, "", message);
    hadError = true;
}

void ErrorHandler::error(Token& token, std::string& message) {
	if (token.type == TokenType::END_OF_FILE) {
      report(token.line, token.col, " at end", message);
    } else {
      report(token.line, token.col, " at '" + token.lexeme + "'", message);
    }

    hadError = true;
}

void ErrorHandler::report(int line, int col, const std::string& where, const std::string& message) const {
	std::cerr << "[line " + std::to_string(line) + " col " + std::to_string(col) + "] Error" + 
		where + ": " + message
		<< std::endl;
    
}

