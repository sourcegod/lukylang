#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "lukerror.hpp"
#include "token.hpp"

// namespace luky {
// forward declarations
// class LukError;

class Scanner {
  public:
    // Scanner(const std::string& _source, LukError& _lukErr);
    Scanner(const std::string& _source, LukError& _lukErr);
    std::vector<Token> scanTokens();

  private:
    size_t start;
    size_t current;
    size_t line;
    size_t col;
    std::string source;
    std::vector<Token> tokens;
    LukError& lukErr;
    const std::string errTitle = "ScanError: ";

    char advance();
    void scanToken();
    void addToken(TokenType);
    void addToken(TokenType, const std::string&);

    bool isAtEnd() const;
    bool match(char);
    char peek() const;
    char peekNext() const;
    bool isDigit(char) const;
    bool isAlpha(char) const;
    bool isAlNum(char) const;
    void string();
    void number();
    void identifier();

   
    // Reserved keywords
    std::unordered_map<std::string, TokenType> keywords;
};
// }

#endif // SCANNER_HPP
