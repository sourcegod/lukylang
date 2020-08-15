#include "scanner.hpp"
#include "lukerror.hpp"

Scanner::Scanner(const std::string& _source, LukError& _lukErr)
      : start(0), current(0),
      line(1), col(1),
      source(_source), lukErr(_lukErr) {
    
    logMsg("\nIn Scanner constructor");
    // initialize reserved keywords map
    keywords["and"]    = TokenType::AND;
    keywords["break"]    = TokenType::BREAK;
    keywords["class"]  = TokenType::CLASS;
    keywords["continue"]    = TokenType::CONTINUE;
    keywords["else"]   = TokenType::ELSE;
    keywords["false"]  = TokenType::FALSE;
    keywords["for"]    = TokenType::FOR;
    keywords["fun"]    = TokenType::FUN;
    keywords["if"]     = TokenType::IF;
    keywords["nil"]    = TokenType::NIL;
    keywords["or"]     = TokenType::OR;
    keywords["print"]  = TokenType::PRINT;
    keywords["return"] = TokenType::RETURN;
    keywords["super"]  = TokenType::SUPER;
    keywords["this"]   = TokenType::THIS;
    keywords["true"]   = TokenType::TRUE;
    keywords["var"]    = TokenType::VAR;
    keywords["while"]  = TokenType::WHILE;
}

char Scanner::advance() {
    current++;
    col++;
    return source[current - 1];
}

void Scanner::scanToken() {
    const char c = advance();
    switch (c) {
        case '(':
            addToken(TokenType::LEFT_PAREN);
            break;
        case ')':
            addToken(TokenType::RIGHT_PAREN);
            break;
        case '{':
            addToken(TokenType::LEFT_BRACE);
            break;
        case '}':
            addToken(TokenType::RIGHT_BRACE);
            break;
        case ',':
            addToken(TokenType::COMMA);
            break;
        case '.':
            addToken(TokenType::DOT);
            break;
        case '-':
            addToken(TokenType::MINUS);
            break;
        case '+':
            addToken(TokenType::PLUS);
            break;
        case ';':
            addToken(TokenType::SEMICOLON);
            break;
        case '*':
            addToken(TokenType::STAR);
            break;
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL
                                          : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL
                                          : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL
                                          : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL
                                          : TokenType::GREATER);
            break;
        case '/':
            if (match('/')) {
                // a comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd())
                    advance();
            } else {
                addToken(TokenType::SLASH);
            }
            break;
        case '"':
            string();
            break;
        case ' ':
        case '\r':
        case '\t':
            // ignore whitespace
            break;
        case '\n':
            line++;
            col =1;
            break;
        default: {
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                std::string errMessage = "Unexpected character: ";
                errMessage += c;
                lukErr.error(errTitle, line, col, errMessage);
                break;
            }
        }
    }
}

bool Scanner::isAlpha(const char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isAlNum(const char c) const {
    return isAlpha(c) || isDigit(c);
}

void Scanner::identifier() {
    // using "maximal munch"
    // e.g. match "orchid" not "or" keyword and "chid"
    while (isAlNum(peek()))
        advance();
    // see if the identifier is a reserved keyword
    const size_t idLen = current - start;
    const std::string identifier  = source.substr(start, idLen);
        if ( keywords.find(identifier) != keywords.end() ) {
            addToken(keywords[identifier]);
    } else {
        addToken(TokenType::IDENTIFIER);
    }
}

bool Scanner::isDigit(const char c) const { return c >= '0' && c <= '9'; }

void Scanner::number() {
    while (isDigit(peek()))
        advance();
    // look for fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        // consume the "."
        advance();
        while (isDigit(peek()))
            advance();
    }
    const size_t numLen = current - start;
    const std::string numberLiteral = source.substr(start, numLen);
    addToken(TokenType::NUMBER, numberLiteral);
}

void Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
            col=1;
        }
        advance();
    }
    // unterminated string
    if (isAtEnd()) {
        lukErr.error(errTitle, line, col, "Unterminated string.");
        return;
    }
    // closing "
    advance();
    const size_t stringLen = current - start;
    // trim the surrounding quotes
    const std::string stringLiteral = source.substr(start + 1, stringLen - 2);
    addToken(TokenType::STRING, stringLiteral);
}

void Scanner::addToken(const TokenType type, const std::string& literal) {
    // FIXE: manage the right lexeme
    const size_t lexLen = current - start;
    // avoid string copy
    auto lexeme = literal;
    if (type != TokenType::IDENTIFIER ||
            type != TokenType::NUMBER ||
            type != TokenType::STRING) {
        lexeme = source.substr(start, lexLen);
    }
    
    // Note: can  pass directly a new pointer to push_back function, without create the pointer before.
    m_tokens.push_back( std::make_shared<Token>(type, lexeme, literal, line, col) );
}

void Scanner::addToken(const TokenType _tokenType) { 
    addToken(_tokenType, ""); 
}

bool Scanner::isAtEnd() const { return current >= source.size(); }

bool Scanner::match(const char expected) {
    if (isAtEnd())
        return false;
    if (source[current] != expected)
        return false;
    current++;
    return true;
}

char Scanner::peekNext() const {
    if (current + 1 >= source.length())
        return '\0';
    return source[current + 1];
}
char Scanner::peek() const {
    if (isAtEnd())
        return '\0';
    return source[current];
}

const std::vector<TokPtr>&& Scanner::scanTokens() {
    while (!isAtEnd()) {
        // we are at the beginning of the next lexeme
        start = current;
        scanToken();
    }
    TokPtr endOfFile = std::make_shared<Token>(TokenType::END_OF_FILE, "EOF", "", line, col);
    // Note: it will be safer to move the pointer to the vector
    m_tokens.push_back( std::move(endOfFile) );
    
    // Note: move function must be used when returning vector of pointer.
    return std::move(m_tokens);
}
