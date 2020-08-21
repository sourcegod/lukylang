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


void LukError::error(const std::string& title, TokPtr& tok, 
        const std::string& message) {
	if (tok->type == TokenType::END_OF_FILE) {
      report(title, tok->line, tok->col, " at end", message);
    } else {
      report(title, tok->line, tok->col, " at '" + tok->lexeme + "'", 
              message);
    }

    hadError = true;
}

void LukError::error(const std::string& title, const std::string& message) {
  std::cerr << title << message 
    << std::endl;

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

