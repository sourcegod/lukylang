#include "parser.hpp"
#include "lukerror.hpp"
#include <vector>
#include <typeinfo>

ParseError::ParseError(const std::string& msg, Token& token)
    : std::runtime_error(msg)
    , m_token(token) {}

struct LargeData {
    int id;
    int ar [100];
};

Parser::Parser(const std::vector<Token>& _tokens, LukError& _lukErr)
    : current(0)
    , tokens(_tokens)
    , lukErr(_lukErr) {}

std::vector<PStmt> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    try {
        while (!isAtEnd()) {
        statements.emplace_back(declaration() );
        }
    } catch(ParseError err) {
            std::cerr << "ParseError: " << err.what() << std::endl;
    }

        
    return statements;
    
}

PStmt Parser::statement() {
    if (match({TokenType::PRINT})) 
        return printStatement();
    
    return  expressionStatement();
}

PStmt Parser::printStatement() {
    PExpr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return PStmt(new PrintStmt(std::move(value)) );
}

PStmt Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    PExpr initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    
    return PStmt(new VarStmt(name, std::move(initializer)) );
}

PStmt Parser::expressionStatement() {
    PExpr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return PStmt(new ExpressionStmt(std::move(expr)) );
}


PExpr Parser::expression() {
    return assignment();
}

PExpr Parser::assignment() {
    PExpr left = equality();
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        PExpr value = assignment();
        if ( left->isVariableExpr() ) {
            Token name = static_cast<VariableExpr*>( left.get() )->name;
            return PExpr(new AssignExpr(name, std::move(value)));
        }
        
        error(equals, "Invalid assignment target.");
    }

    return left;
}

PStmt Parser::declaration() {
    try {
        if (match({TokenType::VAR})) return varDeclaration();
        
        return statement();
    } catch (ParseError err) {
        synchronize();
        return nullptr;
    }

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
        PExpr right = unary();
        expr = PExpr(new BinaryExpr(std::move(expr), Operator, std::move(right)));
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
                TokenType::NUMBER, TokenType::STRING})) {

        return PExpr(new LiteralExpr( LukObject( previous() ) ));
    }

    if (match({TokenType::IDENTIFIER})) {
        return PExpr(new VariableExpr(previous()) );
    }

    if (match({TokenType::LEFT_PAREN})) {
        PExpr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Exppect ')' after expression.");
        return PExpr(new GroupingExpr(std::move(expr)));
    }
    
    
    throw error(peek(), "Expect expression.");
    return nullptr;
}

Token Parser::consume(TokenType type, std::string message) {
    if (check(type))
        return advance();
    throw error(peek(), message);
    
}

ParseError Parser::error(Token& token, const std::string& message) {
    if (token.type == TokenType::END_OF_FILE) {
        lukErr.error(token.line, token.col, " at end, " + message);
    } else {
        lukErr.error(token.line, token.col, "at '" + token.lexeme + "' " + message);
    }
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

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;

        switch(peek().type) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            
            default: break;
        }

        advance();
    }

}
