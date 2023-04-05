#include "scanner.hpp"
#include "lukerror.hpp"
using namespace luky;
// static variables
LukError lukErr;
Scanner Scanner::m_scan = Scanner(lukErr);

Scanner::Scanner(const std::string& source, LukError& lukErr)
        : m_start(0), m_current(0),
        m_line(1), m_col(0),
        m_source(source), m_lukErr(lukErr), 
        m_addingEOF(true) {
    logMsg("\nIn Scanner constructor");
    initKeywords();
}

Scanner::Scanner(LukError& lukErr)
        : m_start(0), m_current(0),
        m_line(1), m_col(0),
        m_source(""), m_lukErr(lukErr),
        m_addingEOF(false) {
    logMsg("\nIn Second Scanner constructor");
    initKeywords();
}


void Scanner::initKeywords() {
    // initialize reserved m_keywords map
    m_keywords["and"]    = TokenType::AND;
    m_keywords["break"]    = TokenType::BREAK;
    m_keywords["class"]  = TokenType::CLASS;
    m_keywords["continue"]    = TokenType::CONTINUE;
    m_keywords["do"]   = TokenType::DO;
    m_keywords["else"]   = TokenType::ELSE;
    m_keywords["false"]  = TokenType::FALSE;
    m_keywords["for"]    = TokenType::FOR;
    m_keywords["fun"]    = TokenType::FUN;
    m_keywords["if"]     = TokenType::IF;
    m_keywords["nil"]    = TokenType::NIL;
    m_keywords["or"]     = TokenType::OR;
    m_keywords["print"]  = TokenType::PRINT;
    m_keywords["return"] = TokenType::RETURN;
    m_keywords["super"]  = TokenType::SUPER;
    m_keywords["this"]   = TokenType::THIS;
    m_keywords["true"]   = TokenType::TRUE;
    m_keywords["var"]    = TokenType::VAR;
    m_keywords["while"]  = TokenType::WHILE;

}

void Scanner::initScan(const std::string& source, size_t line, size_t col, bool addingEOF) {
    // init global params
    m_start = m_current =0;
    m_source = source;
    m_line = line;
    m_col = col;
    m_addingEOF = addingEOF;
    m_tokens.clear();

}

void Scanner::addToken(const TokenType _tokenType) { 
    addToken(_tokenType, ""); 
}


void Scanner::addToken(const TokenType type, const std::string& literal) {
    // FIXE: manage the right lexeme
    const size_t lexLen = m_current - m_start;
    // avoid string copy
    auto lexeme = literal;
    if (type != TokenType::IDENTIFIER ||
            type != TokenType::NUMBER ||
            type != TokenType::STRING ||
            type != TokenType::INT ||
            type != TokenType::DOUBLE) { 
        lexeme = m_source.substr(m_start, lexLen);
    }
    // std::cerr << "Token lexeme: " << lexeme << ", literal: " << literal << "\n";   
    // Note: can  pass directly a new pointer to push_back function, without create the pointer before.
    m_tokens.push_back( std::make_shared<Token>(type, lexeme, literal, m_line, m_col) );
}

void Scanner::insertToken(const TokenType type, const std::string& literal) { 
    auto lexeme = literal;
    m_col++;
    m_tokens.push_back( std::make_shared<Token>(type, lexeme, literal, m_line, m_col) );

}

void Scanner::addToken(const TokenType type, const std::string& lexeme, std::string literal) {
    if (literal == "") literal = lexeme;
    // Note: can  pass directly a new pointer to push_back function, without create the pointer before.
    m_tokens.push_back( std::make_shared<Token>(type, lexeme, literal, m_line, m_col) );
}

void Scanner::scanToken() {
    const char ch = advance();
    switch (ch) {
        case '(': addToken(TokenType::LEFT_PAREN); break;
        case ')': addToken(TokenType::RIGHT_PAREN); break;
        case '{': addToken(TokenType::LEFT_BRACE); break;
        
        // Automatic semicolon insertion
        case '}': 
            lastToken = m_tokens.back();
            if (lastToken->type != TokenType::LEFT_BRACE &&
                    lastToken->type != TokenType::RIGHT_BRACE && 
                    lastToken->type != TokenType::SEMICOLON) {
                insertToken(TokenType::SEMICOLON, ";");
            }
            addToken(TokenType::RIGHT_BRACE); 
            break;
        case ',': addToken(TokenType::COMMA); break;
        case '.': addToken(TokenType::DOT); break;
        case ';': addToken(TokenType::SEMICOLON); break;

        // Adding: prefix and postfix operators  
        case '-': 
            if (match('=')) addToken(TokenType::MINUS_EQUAL);
            else addToken(match('-') ? TokenType::MINUS_MINUS : TokenType::MINUS);
            break;
      
        case '+': 
            if (match('=')) addToken(TokenType::PLUS_EQUAL);
            else addToken(match('+') ? TokenType::PLUS_PLUS : TokenType::PLUS);
            break;
                
        case '*': 
            if (match('=')) addToken(TokenType::STAR_EQUAL);
            else if (match('*')) {
                if (match('=')) addToken(TokenType::EXP_EQUAL);
                else addToken(TokenType::EXP);
            }
            else addToken(TokenType::STAR);
            break; 

        case '/':
            if (match('/')) {
                // a comment goes until the end of the m_line.
                skipComments();
                // while (peek() != '\n' && !isAtEnd()) advance();
            } else if (match('*')) {
                skipMultilineComments();
            } else {
                addToken(match('=') ? TokenType::SLASH_EQUAL : TokenType::SLASH);
            }
            break;

        case '%': addToken(match('=') ? TokenType::MOD_EQUAL : TokenType::MOD); break;

        case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
        case '=': 
          if (match('=')) addToken(TokenType::EQUAL_EQUAL);
          else if (match('>')) addToken(TokenType::EQUAL_ARROW);
          else addToken(TokenType::EQUAL); 
          break;

        // Adding: bitwise shift operators
        case '<': 
          if (match('=')) addToken(TokenType::LESSER_EQUAL);
          else if (match('<')) {
              if (match('=')) addToken(TokenType::BIT_LEFT_EQUAL);
              else addToken(TokenType::BIT_LEFT);
          }
          else addToken(TokenType::LESSER); 
          break; 

        case '>': 
          if (match('=')) addToken(TokenType::GREATER_EQUAL);
          else if (match('>')) {
              if (match('=')) addToken(TokenType::BIT_RIGHT_EQUAL);
              else addToken(TokenType::BIT_RIGHT);
          }
          else addToken(TokenType::GREATER); 
          break; 

        case ':': addToken(TokenType::COLON); break;
        case '?': addToken(TokenType::QUESTION); break;
      
        // bitwise operators
        case '&': addToken(match('=') ? TokenType::BIT_AND_EQUAL : TokenType::BIT_AND); break;
        case '|': addToken(match('=') ? TokenType::BIT_OR_EQUAL : TokenType::BIT_OR); break;
        case '~': addToken(TokenType::BIT_NOT); break;
        case '^': addToken(match('=') ? TokenType::BIT_XOR_EQUAL : TokenType::BIT_XOR); break;

        case ' ':
        case '\r':
        case '\t':
            // ignore whitespace
            break;
        case '\n':
            m_line++;
            m_col =0;
            // Automatic semicolon insertion
            if (m_tokens.size() == 0) break;
            lastToken = m_tokens.back();
            // No insert semicolon 
            if (lastToken->type == TokenType::RIGHT_PAREN && 
                    searchPrintable() == '{' ) {
                break;
            } else if (lastToken->type !=  TokenType::SEMICOLON &&
                    lastToken->type != TokenType::LEFT_BRACE &&
                    lastToken->type != TokenType::RIGHT_BRACE ) {
                insertToken(TokenType::SEMICOLON, ";");
            }
            break;


        // support simple and double quotes string
        case '"': addString(ch); break;
        case '\'': addString(ch); break;

        default: {
            if (isDigit(ch)) {
                number();
            } else if (isAlpha(ch)) {
                identifier();
            } else {
                std::string errMessage = "Unexpected character: ";
                errMessage += ch;
                m_lukErr.error(m_errTitle, m_line, m_col, errMessage);
                break;
            }
        }
    }
}

const std::vector<TokPtr>&& Scanner::scanTokens() {
    while (!isAtEnd()) {
        // we are at the beginning of the next lexeme
        m_start = m_current;
        scanToken();
    }
    // Adding End Of File token?
    if (m_addingEOF) {
        TokPtr endOfFile = std::make_shared<Token>(TokenType::END_OF_FILE, "EOF", "", m_line, m_col);
        // Note: it will be safer to move the pointer to the vector
        m_tokens.push_back( std::move(endOfFile) );
    }

#ifdef DEBUG
      logTokens();
#endif

    // Note: move function must be used when returning vector of pointer.
    return std::move(m_tokens);
}

char Scanner::advance() {
    m_current++;
    m_col++;
    return m_source[m_current - 1];
}

bool Scanner::isAtEnd() const { return m_current >= m_source.size(); }

void Scanner::identifier() {
    // using "maximal munch"
    // e.g. match "orchid" not "or" keyword and "chid"
    while (isAlNum(peek()))
        advance();
    // see if the identifier is a reserved keyword
    const size_t idLen = m_current - m_start;
    const std::string identifier  = m_source.substr(m_start, idLen);
        if ( m_keywords.find(identifier) != m_keywords.end() ) {
            addToken(m_keywords[identifier]);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

void Scanner::number() {
  bool isDecimal = false;
    while (isDigit(peek()))
        advance();
    // look for fractional part
    if (peek() == '.' && isDigit(peekNext())) {
      isDecimal = true;
        // consume the "."
        advance();
        while (isDigit(peek()))
            advance();
    }
    const size_t numLen = m_current - m_start;
    const std::string numLiteral = m_source.substr(m_start, numLen);
    if (not isDecimal) 
        addToken(TokenType::INT, numLiteral);
    else
        addToken(TokenType::DOUBLE, numLiteral);
}

std::string Scanner::unescape(const std::string& escaped) {
    // escape sequence character
    std::string strChar;
    
    for (size_t i=0; i < escaped.size(); i++) {
        if (escaped[i] == '\\') {
            i++;
            switch (escaped[i]) {
                case 'n': strChar.push_back('\n'); break;
                case 'r': strChar.push_back('\r'); break;
                case '\\': strChar.push_back('\\'); break;
                case '"': strChar.push_back('\"'); break;
                case '\'': strChar.push_back('\''); break;
                case 't': strChar.push_back('\t'); break;
                case 'b': strChar.push_back('\b'); break;
                case '$': strChar.push_back('$'); break;
                case '{': strChar.push_back('{'); break;
                case '}': strChar.push_back('}'); break;
                
                default:
                    /// Note: best way to construct string with const char* 
                    /// is to create first char* in a std::string
                    m_lukErr.error(m_errTitle, m_line, m_col, 
                          std::string("Unrecognized escape sequence : '\\") +
                          escaped[i] + "'."); 
            } 
        
        } else {
            strChar.push_back(escaped[i]);
        }
    
    }
        
    return  strChar;
}

void Scanner::addString(char ch) {
    // the ch argument is to indicate whether it's simple or double quotes
    bool isInterp = false;
    synchronize();
    while (peek() != ch && !isAtEnd()) {
        auto curChar = peek();
        auto nextChar = peekNext();
        if (curChar == '\n') {
            m_line++;
            m_col=0;
        }
        if (curChar == '\\' && (
              nextChar  == ch || nextChar == '$') ) advance();

        // searching interpolation expression
        if ( isStartIdent(curChar) || isStartExpr(curChar) ) {
            isInterp = true;
            auto part = unescape(getPart());
            addToken(TokenType::STRING, unescape(part));
            addToken(TokenType::INTERP_PLUS, "_+", "");
            synchronize();

            if (isIdent(nextChar)) { // interpolation identifier
                auto ident = getIdent();
                addToken(TokenType::IDENTIFIER, ident);
                synchronize();

            } else if (isExpr(nextChar)) { // interpolation expression
                auto expr = getExpr();
                scanInterpExpr(expr);
                synchronize();
            }
           
            // cannot use curChar or nextChar 
            // cause current char has been changed by getIdent or getExpr function.
            if (peek() != ch && !isAtEnd() ) {
                addToken(TokenType::INTERP_PLUS, "_+", "");
            }
            continue;

        }
       if (!isAtEnd())  advance();

    } // End While
    
    // unterminated string
    if (isAtEnd()) {
        m_lukErr.error(m_errTitle, m_line, m_col, std::string("Unterminated string: '") + ch + "'.");
        return;
    }
    
    // the closing "
    advance();
    // Handle escapes sequences
    // trim the surrounding quotes
    const size_t strLen = m_current - m_start;
    /// Note: returns whether there is no string after the Interpolating String
    if (strLen == 1 && isInterp) return;
    // if (strLen > 1 && isInterp) { // whether is not only '"' char
    else {
        const std::string strLiteral = unescape(m_source.substr(m_start, strLen -1));
        addToken(TokenType::STRING, strLiteral);
    }
    

}

bool Scanner::match(const char expected) {
    if (isAtEnd())
        return false;
    if (m_source[m_current] != expected)
        return false;
    m_current++;
    return true;
}

char Scanner::peek() const {
    if (isAtEnd())
        return '\0';
    return m_source[m_current];
}

char Scanner::peekNext() const {
    if (m_current + 1 >= m_source.length())
        return '\0';
    return m_source[m_current + 1];
}

bool Scanner::isDigit(const char c) const { return c >= '0' && c <= '9'; }

bool Scanner::isAlpha(const char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isAlNum(const char c) const {
    return isAlpha(c) || isDigit(c);
}

void Scanner::skipComments() {
    while (!isAtEnd()) {
        if (peek() != '\n') {
            advance();
        } else {
            m_line++;
            m_col =0;
            return;
        }
    }

}


void Scanner::skipMultilineComments() {
    while (!isAtEnd()) {
        advance();
        if (peek() == '\n') {
            m_line++;
            m_col =0;
            continue;
        }
        if (peek() == '/' && peekNext() == '*') skipMultilineComments();
        if (match('*') && match('/')) break;
    }

}

bool Scanner::isPrintable(char ch) {
    return (ch != ' ' && 
        ch != '\n' && 
        ch != '\r' && 
        ch != '\t' );
}

char Scanner::searchPrintable() {
    if (isAtEnd()) return '\0';
    size_t cur = m_current;
    char ch;
    while (cur < m_source.size()) {
        ch = m_source[cur];
        if (isPrintable(ch)) return ch;
        cur++;
    }

    return '\0';
}

void Scanner::logTokens() {
  if (m_addingEOF)
      logMsg("Tokens list for Main Scanner");
  else
      logMsg("Tokens list for Second Scanner");
  for (auto& it: m_tokens) {
    logMsg("id: ", it->id, "lexeme: ", it->lexeme);
  }

}

void Scanner::synchronize() {
    m_start = m_col = m_current;
    // logMsg("Synchronizing, start: ", start, ", current: ", current);
}

bool Scanner::isStartIdent(const char c) const {
    return  c == '$'  && (peekNext() == '_' || isAlpha(peekNext()) );
}

bool Scanner::isIdent(const char c) const {
    return isAlNum(c);
}

bool Scanner::isStartExpr(const char c) const {
    return  c == '$' && peekNext() == '{';
}

bool Scanner::isExpr(const char c) const {
    // only '}' char retrieve false cause it is the end of expression
    if (c == '}' || isAtEnd()) return false;
    return true;
}

std::string Scanner::getIdent() {
    // logMsg("\nIn getIdent, start: ", start, ", current: ", current);
    // consume the '$' for the identifier
    if (!isAtEnd()) { 
        advance();
        m_start++;
    }

    while ( isIdent(peek()) ) {
        advance();
    }
    const size_t idLen = m_current - m_start;
    const std::string ident  = m_source.substr(m_start, idLen);
    // logMsg("Exit out  getIdent, with ident: ", ident, "\n");

    return ident;
}

std::string Scanner::getExpr() {
    // logMsg("\nIn getExpr, start: ", start, ", current: ", current);
    // consume the '${' for the expression
    auto oldStart = m_start;
    if (peek() == '$' && peekNext() == '{') { 
        advance(); advance();
        m_start +=2;
    }

    while ( isExpr(peek()) ) {
        advance();
    }

    if (isAtEnd()) {
        auto errExpr = m_source.substr(oldStart, (m_current -1) - oldStart);
        m_lukErr.error(m_errTitle, m_line, m_col, 
            std::string("Unterminated Interpolating Expression: '") + errExpr + "'");
        return errExpr;
    }

    // consume the '}' for the end of expression
    if (peek() == '}') advance();
    // unterminated interpolating expression
    const size_t exLen = (m_current -1) - m_start;
    const std::string expr  = m_source.substr(m_start, exLen);
    // logMsg("Exit out  getExpr, with expr: ", expr, "\n");

    return expr;
}


std::string Scanner::getPart() {
    const size_t strLen = m_current - m_start;
    // logMsg("\nIn addpart, start: ", start);
    // logMsg("current: ", current, ", len: ", stringLen);
    
    // trim the surrounding quotes

    return m_source.substr(m_start, strLen);
}
void Scanner::scanInterpExpr(const std::string& expr) {
  // rescanning interpolating expression
    m_scan.initScan(expr, m_line, m_col, false);
    auto v_tok = m_scan.scanTokens();
    /// Note: append v_tok into m_tokens without copy
    std::move(v_tok.begin(), v_tok.end(), std::back_inserter(m_tokens));

}
  
