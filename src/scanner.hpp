#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "lukerror.hpp"
#include "token.hpp"

namespace luky {
    // class Scanner;
    class Scanner {
    public:
        Scanner(const std::string& source, LukError& lukErr);
        Scanner(LukError& lukErr);
        
        ~Scanner() {
          logMsg("\n~Scanner destructor");
        }    
        
        void initScan(const std::string& source, size_t line, size_t col, bool addingEOF);
        const std::vector<TokPtr>&& scanTokens();

      private:
        size_t m_start;
        size_t m_current;
        size_t m_line;
        size_t m_col;
        std::string m_source;
        std::vector<TokPtr> m_tokens;
        TokPtr lastToken;
        LukError& m_lukErr;
        const std::string m_errTitle = "ScanError: ";
        // Reserved keywords
        std::unordered_map<std::string, TokenType> m_keywords;
        bool m_addingEOF;
        /// Note: cannot put an instance of a class into itself
        /// that would result in infinite recursion
        /// Note: Use pointer to a class instead
        /// Scanner* m_scan; 
        /// or use static class
        static Scanner m_scan;


        void initKeywords();
        void addToken(TokenType);
        void addToken(TokenType, const std::string&);
        void addToken(TokenType, const std::string& lexeme, std::string literal);
        void insertToken(const TokenType type, const std::string& literal);

        void scanToken();
        char advance();
        bool isAtEnd() const;
        void identifier();
        void number();
        std::string unescape(const std::string& escaped);
        void addString(char ch='"');
        bool match(char);
        char peek() const;
        char peekNext() const;
        bool isDigit(char) const;
        bool isAlpha(char) const;
        bool isAlNum(char) const;
        void skipComments();
        void skipMultilineComments();
        bool isPrintable(char ch);
        char searchPrintable();
        void logTokens();
        
        // String Interpolation functions
        void synchronize();
        bool isStartIdent(const char c) const;
        bool isIdent(const char c) const;
        bool isStartExpr(const char c) const;
        bool isExpr(const char c) const;
        std::string getIdent();
        std::string getExpr();
        std::string getPart();
        void scanInterpExpr(const std::string& expr);
       
    };
}

#endif // SCANNER_HPP
