#include "parser.hpp"
#include "lukerror.hpp"
#include <vector>
#include <typeinfo>
#include <memory>

ParseError::ParseError(const std::string& msg, Token& token)
    : std::runtime_error(msg)
    , m_token(token) {}

Parser::Parser(const std::vector<Token>&& tokens, LukError& _lukErr)
    : current(0)
    , m_tokens(std::move(tokens))
    , lukErr(_lukErr) {}

std::vector<PStmt> Parser::parse() {
    std::vector<PStmt> statements;
    try {
        while (!isAtEnd()) {
        statements.emplace_back(declaration() );
        }
    } catch(ParseError err) {
            std::cerr << errTitle << err.what() << std::endl;
    }

        
    return statements;
    
}

PStmt Parser::statement() {
    if (match({TokenType::BREAK, TokenType::CONTINUE})) 
        return breakStatement();

    if (match({TokenType::FOR})) 
        return forStatement();
    if (match({TokenType::IF})) 
        return ifStatement();
    if (match({TokenType::PRINT})) 
        return printStatement();
    if (match({TokenType::RETURN})) 
        return returnStatement();
    if (match({TokenType::WHILE})) 
        return whileStatement();
    if (match({TokenType::LEFT_BRACE}))
        return std::make_shared<BlockStmt>( block() );
    
    return  expressionStatement();
}

std::vector<PStmt> Parser::block() {
    std::vector<PStmt> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.emplace_back( declaration() );
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");

    return statements;
}


PStmt Parser::breakStatement() {
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after break statement");
    return std::make_shared<BreakStmt>(keyword);
}

PStmt Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'");
    PStmt initializer; 
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    PExpr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    
    PStmt increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = std::make_shared<ExpressionStmt>(expression() );
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    PStmt body = statement();

    if (increment != nullptr) {
        std::vector<PStmt> stmts;
        stmts.push_back(body);
        stmts.push_back(increment);
        body = std::make_shared<BlockStmt>( stmts );
    }
    body = std::make_shared<WhileStmt>(condition, body);
    if (initializer) {
        std::vector<PStmt> stmts;
        stmts.push_back( initializer );
        stmts.push_back( body );
        return std::make_shared<BlockStmt>(stmts);
    }

    return body;
}

PStmt Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'");
    PExpr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");
    PStmt thenBranch = statement();
    PStmt elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_shared<IfStmt>(condition, thenBranch,
                elseBranch);
}

PStmt Parser::printStatement() {
    PExpr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_shared<PrintStmt>(value);
}

PStmt Parser::returnStatement() {
    Token keyword = previous();
    PExpr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");

    return std::make_shared<ReturnStmt>(keyword, value);
}


PStmt Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'");
    PExpr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition");
    PStmt body = statement();

    return std::make_shared<WhileStmt>(condition, body);
}

PStmt Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    PExpr initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    
    return std::make_shared<VarStmt>(name, initializer);
}

PStmt Parser::classDeclaration() {
    // TODO: convert all unique_ptr to shared_ptr 
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after class body.");
    
    std::vector<PStmt> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
      methods.push_back(function("method"));

    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
  
    return std::make_shared<ClassStmt>(name, methods);
}

PStmt Parser::declaration() {
    try {
        if  (match({TokenType::CLASS})) return classDeclaration();
        if (match({TokenType::FUN})) return function("function");
        if (match({TokenType::VAR})) return varDeclaration();
        
        return statement();
    } catch (ParseError err) {
        synchronize();
        return nullptr;
    }

}

PStmt Parser::expressionStatement() {
    PExpr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStmt>(expr);
}

PStmt Parser::function(std::string kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (params.size() >= 8) {
                error(peek(), "Cannot have more than 8 parameters.");
            }
            
            params.emplace_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA}));
    }

    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");

    std::vector<PStmt> body = block();
    
    return std::make_shared<FunctionStmt>(name, params, body);

}


PExpr Parser::expression() {
    return assignment();
}

PExpr Parser::assignment() {
    PExpr left = logicOr();
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        PExpr value = assignment();
        if ( left->isVariableExpr() ) {
            // Token name = static_cast<VariableExpr*>( left.get() )->name;
            Token name = left->getName();
            return  std::make_shared<AssignExpr>(name, value);
        } else if (left->isGetExpr()) {
          return std::make_shared<SetExpr>(left->getObject(),
                left->getName(), value );
        }
        
        error(equals, "Invalid assignment target.");
    }

    return left;
}

PExpr Parser::logicOr() {
    PExpr left = logicAnd();
    while (match({TokenType::OR})) {
        Token op = previous();
        PExpr right = logicAnd();
        left =  std::make_shared<LogicalExpr>(left, op, right);
    }

    return left;
}

PExpr Parser::logicAnd() {
    PExpr left = equality();
    while (match({TokenType::AND})) {
        Token op = previous();
        PExpr right = equality();
        left =  std::make_shared<LogicalExpr>(left, op, right);
    }

    return left;
}

PExpr Parser::equality() {
    PExpr expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        PExpr right    = comparison();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    return expr;
}

PExpr Parser::comparison() {
    PExpr expr = addition();
    while (
        match({TokenType::GREATER, TokenType::LESS, 
            TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL})) {
        Token op = previous();
        PExpr right = addition();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    return expr;
}

PExpr Parser::addition() {
    PExpr expr = multiplication();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token Operator = previous();
        PExpr right = multiplication();
        expr = std::make_shared<BinaryExpr>(expr, Operator, right);
    }
    return expr;
}

PExpr Parser::multiplication() {
    PExpr expr = unary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token Operator = previous();
        PExpr right = unary();
        expr = std::make_shared<BinaryExpr>(expr, Operator, right);
    }
    return expr;
}

PExpr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token Operator = previous();
        PExpr right    = unary();
        return std::make_shared<UnaryExpr>(Operator, right);
    }

    return call();
}

PExpr Parser::call() {
    PExpr expr = primary();
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(expr);
        } else if (match({TokenType::DOT})) {
          Token name = consume(TokenType::IDENTIFIER,
            "Expect property name after '.'.");
          expr = std::make_shared<GetExpr>(expr, name);
        } else {
            break;
        }
    }

    return expr;
}

PExpr Parser::finishCall(PExpr callee) {
    std::vector<PExpr> args;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (args.size() >= 8) {
                error(peek(), "Cannot have more than 8 arguments.");
            }
            args.emplace_back(expression());
        } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<CallExpr>(callee, paren, args);
}

PExpr Parser::primary() {
    if (match(
                {TokenType::FALSE, TokenType::TRUE, 
                TokenType::NIL,
                TokenType::NUMBER, TokenType::STRING})) {
        const auto obj = LukObject( previous() );
        // std::cerr << "Parser::Primary, obj.p_string: " << obj.p_string << std::endl;
        return std::make_shared<LiteralExpr>( obj );
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<VariableExpr>(previous());
    }

    if (match({TokenType::LEFT_PAREN})) {
        PExpr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Exppect ')' after expression.");
        return std::make_shared<GroupingExpr>(expr);
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
        lukErr.error(errTitle, token.line, token.col, " at end, " + message);
    } else {
        lukErr.error(errTitle, token.line, token.col, 
                "at '" + token.lexeme + "' " + message);
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
    return m_tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd())
        ++current;
    return previous();
}

Token& Parser::peek() {
    return m_tokens[current];
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
