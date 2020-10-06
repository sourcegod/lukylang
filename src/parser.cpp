#include "parser.hpp"
#include "lukerror.hpp"
#include <vector>

// using namespace luky;

ParseError::ParseError(const std::string& msg, Token& token)
    : std::runtime_error(msg)
    , m_token(token) {}

Parser::Parser(const std::vector<Token>& _tokens, LukError& _lukErr)
    : current(0)
    , tokens(_tokens)
    , lukErr(_lukErr) {}

PExpr Parser::expression() {
    return equality();
}

PExpr Parser::equality() {
    PExpr expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        PExpr right    = comparison();
        expr           = PExpr(new BinaryExpr(std::move(expr), op, std::move(right)));
    }
    return expr;
}

PExpr Parser::comparison() {
    PExpr expr = addition();
    while (
        match({TokenType::GREATER, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        PExpr right    = addition();
        expr           = PExpr(new BinaryExpr(std::move(expr), op, std::move(right)));
    }
    return expr;
}

PExpr Parser::addition() {
    PExpr expr = multiplication();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token Operator = previous();
        PExpr right    = multiplication();
        expr           = PExpr(new BinaryExpr(std::move(expr), Operator, std::move(right)));
    }
    return expr;
}

PExpr Parser::multiplication() {
    PExpr expr = unary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token Operator = previous();
        PExpr right    = unary();
        expr           = PExpr(new BinaryExpr(std::move(expr), Operator, std::move(right)));
    }
    return expr;
}

PExpr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token Operator = previous();
        PExpr right    = unary();
        return PExpr(new UnaryExpr(Operator, std::move(right)));
    }
    return primary();
}

PExpr Parser::primary() {
    if (match(
                {TokenType::FALSE, TokenType::TRUE,
                TokenType::NIL, 
                TokenType::NUMBER, TokenType::STRING}))
        // std::cout << "parser: " << previous().lexeme << std::endl;
        // LukObject obj(previous());
        // std::cout << "voici : " << obj.m_number << std::endl;
        return PExpr(new LiteralExpr( LukObject( previous() ) ));

    if (match({TokenType::LEFT_PAREN})) {
        PExpr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Exppect ')' after expression.");
        return PExpr(new GroupingExpr(std::move(expr)));
    }
    
    
    throw error(peek(), "Expect expression.");
    return nullptr;
}

PExpr Parser::parse() {
    try {
        return expression();
    } catch (ParseError error) {
        return nullptr;
    }
}

Token Parser::consume(TokenType type, std::string message) {
    if (check(type))
        return advance();
    throw error(peek(), message);
}

ParseError Parser::error(Token& token, const std::string& message) {
    if (token.type == TokenType::END_OF_FILE) {
        lukErr.error(token.line, token.col, " at end" + message);
    } else {
        lukErr.error(token.line, token.col, "at '" + token.lexeme + "'" + message);
    }
    // errorHandler_.report();
    return *new ParseError(message, token);
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd())
        ++current;
    return previous();
}

Token& Parser::peek() {
    return tokens[current];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) {
    if (isAtEnd())
        return false;
    return peek().type == type;
}
