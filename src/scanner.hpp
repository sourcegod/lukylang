#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "lukerror.hpp"
#include "token.hpp"


class Scanner {
public:
    Scanner(const std::string& source, LukError& lukErr);
    
    ~Scanner() {
      logMsg("\n~Scanner destructor");
    }    
    
    const std::vector<TokPtr>&& scanTokens();

  private:
    size_t m_start;
    size_t m_current;
    size_t m_line;
    size_t m_col;
    std::string m_source;
    std::vector<TokPtr> m_tokens;
    LukError& m_lukErr;
    const std::string m_errTitle = "ScanError: ";
    // Reserved keywords
    std::unordered_map<std::string, TokenType> m_keywords;


    void addToken(TokenType);
    void addToken(TokenType, const std::string&);

    void scanToken();
    char advance();
    bool isAtEnd() const;
    void identifier();
    void number();
    std::string unescape(const std::string& escaped);
    void string(char ch='"');
    bool match(char);
    char peek() const;
    char peekNext() const;
    bool isDigit(char) const;
    bool isAlpha(char) const;
    bool isAlNum(char) const;
    void skipComments();
    void skipMultilineComments();
    bool isPrintable(char ch);
   
};

#endif // SCANNER_HPP
