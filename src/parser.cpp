# include "parser.hpp"
#include "lukerror.hpp"
#include <vector>
#include <typeinfo>
#include <memory>

ParseError::ParseError(const std::string& msg, TokPtr& tokP)
    : std::runtime_error(msg)
    , m_token(tokP) {}

Parser::Parser(const std::vector<TokPtr>&& tokens, LukError& _lukErr)
      : m_current(0),
      m_tokens(std::move(tokens)),
      lukErr(_lukErr) {
    logMsg("\nIn Parser constructor");
}

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> statements;
    try {
        while (!isAtEnd()) {
        statements.emplace_back(declaration() );
    }
    // Note: catching exception should be by reference, not by value
    } catch(ParseError& err) {
            std::cerr << errTitle << err.what() << std::endl;
    }

        
    return statements;
    
}

StmtPtr Parser::statement() {
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

std::vector<StmtPtr> Parser::block() {
    std::vector<StmtPtr> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.emplace_back( declaration() );
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");

    return statements;
}


StmtPtr Parser::breakStatement() {
    TokPtr keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after break statement");
    return std::make_shared<BreakStmt>(keyword);
}

StmtPtr Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'");
    StmtPtr initializer; 
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    ExprPtr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    
    StmtPtr increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = std::make_shared<ExpressionStmt>(expression() );
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    StmtPtr body = statement();

    if (increment != nullptr) {
        std::vector<StmtPtr> stmts;
        stmts.push_back(body);
        stmts.push_back(increment);
        body = std::make_shared<BlockStmt>( std::move(stmts) );
    }
    body = std::make_shared<WhileStmt>(condition, body);
    if (initializer) {
        std::vector<StmtPtr> stmts;
        stmts.push_back( initializer );
        stmts.push_back( body );
        return std::make_shared<BlockStmt>( std::move(stmts) );
    }

    return body;
}

StmtPtr Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");
    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    return std::make_shared<IfStmt>(condition, thenBranch,
                elseBranch);
}

StmtPtr Parser::printStatement() {
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_shared<PrintStmt>(value);
}

StmtPtr Parser::returnStatement() {
    TokPtr keyword = previous();
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");

    return std::make_shared<ReturnStmt>(keyword, value);
}


StmtPtr Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition");
    StmtPtr body = statement();

    return std::make_shared<WhileStmt>(condition, body);
}

StmtPtr Parser::varDeclaration() {
    TokPtr name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    ExprPtr initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    
    return std::make_shared<VarStmt>(name, initializer);
}

StmtPtr Parser::classDeclaration() {
    TokPtr name = consume(TokenType::IDENTIFIER, "Expect class name.");
    std::shared_ptr<VariableExpr> superclass = nullptr;
    if (match({TokenType::LESS})) {
      consume(TokenType::IDENTIFIER, "Expect superclass name.");
      superclass = std::make_shared<VariableExpr>(previous());
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' after class body.");
    
    std::vector<FuncPtr> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
      methods.push_back( std::move(function("method")) );
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
  
    return std::make_shared<ClassStmt>(name, superclass, std::move(methods) );
}

StmtPtr Parser::declaration() {
    try {
        if  (match({TokenType::CLASS})) return classDeclaration();
        if ( check(TokenType::FUN) && checkNext(TokenType::IDENTIFIER)) {
          consume(TokenType::FUN, "");  
          return function("function");
        }
        
        if (match({TokenType::VAR})) return varDeclaration();
        
        return statement();
    } catch (ParseError& err) {
        synchronize();
        return nullptr;
    }

}

StmtPtr Parser::expressionStatement() {
    ExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStmt>(expr);
}

FuncPtr Parser::function(const std::string& kind) {
    TokPtr name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    return std::make_shared<FunctionStmt>(name, functionBody(kind));
}

std::shared_ptr<FunctionExpr> Parser::functionBody(const std::string& kind) {
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<TokPtr> params;
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

    std::vector<StmtPtr> body = block();

    return std::make_shared<FunctionExpr>(params, body);
}

ExprPtr Parser::expression() {
    return assignment();
}

ExprPtr Parser::assignment() {
    ExprPtr left = logicOr();
    if (match({TokenType::EQUAL})) {
        TokPtr equals = previous();
        ExprPtr value = assignment();
        if ( left->isVariableExpr() ) {
            // TokPtr name = static_cast<VariableExpr*>( left.get() )->name;
            TokPtr name = left->getName();
            return  std::make_shared<AssignExpr>(name, value);
        } else if (left->isGetExpr()) {
          return std::make_shared<SetExpr>(left->getObject(),
                left->getName(), value );
        }
        
        error(equals, "Invalid assignment target.");
    }

    // adding: compound assignment
    if (match({TokenType::PLUS_EQUAL, TokenType::MINUS_EQUAL, 
          TokenType::STAR_EQUAL, TokenType::SLASH_EQUAL, 
          TokenType::MOD_EQUAL})) {
      TokPtr op = previous();
      return compoundAssignment(left, op);
    }

    return left;
}

ExprPtr Parser::compoundAssignment(ExprPtr left, TokPtr op) {
    ExprPtr value = addition();
    if (left->isVariableExpr()) {
        TokPtr name = left->getName();
        ExprPtr val = std::make_shared<BinaryExpr>(left, op, value);
        return std::make_shared<AssignExpr>(name, val);
    }

    error(op, "Invalid compound assignment target.");

    return left;
}

ExprPtr Parser::logicOr() {
    ExprPtr left = logicAnd();
    while (match({TokenType::OR})) {
        TokPtr op = previous();
        ExprPtr right = logicAnd();
        left =  std::make_shared<LogicalExpr>(left, op, right);
    }

    return left;
}

ExprPtr Parser::logicAnd() {
    ExprPtr left = equality();
    while (match({TokenType::AND})) {
        TokPtr op = previous();
        ExprPtr right = equality();
        left =  std::make_shared<LogicalExpr>(left, op, right);
    }

    return left;
}

ExprPtr Parser::equality() {
    ExprPtr left = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        TokPtr op = previous();
        ExprPtr right    = comparison();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::comparison() {
    ExprPtr left = addition();
    while (
        match({TokenType::GREATER, TokenType::LESS, 
            TokenType::LESS_EQUAL, TokenType::GREATER_EQUAL})) {
        TokPtr op = previous();
        ExprPtr right = addition();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::addition() {
    ExprPtr left = multiplication();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        TokPtr op = previous();
        ExprPtr right = multiplication();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::multiplication() {
    ExprPtr left = unary();
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::MOD})) {
        // Note: cannot use "operator" as variable name, cause it's a reserved keyword in C++
        TokPtr op = previous();
        ExprPtr right = unary();
        left = std::make_shared<BinaryExpr>(left, op, right);
    }
    
    return left;
}

ExprPtr Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS, TokenType::PLUS})) {
        TokPtr op = previous();
        ExprPtr right = unary();
        return std::make_shared<UnaryExpr>(op, right);
    }

    return call();
}

ExprPtr Parser::call() {
    ExprPtr expr = primary();
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(expr);
        } else if (match({TokenType::DOT})) {
          TokPtr name = consume(TokenType::IDENTIFIER,
            "Expect property name after '.'.");
          expr = std::make_shared<GetExpr>(expr, name);
        } else {
            break;
        }
    }

    return expr;
}

ExprPtr Parser::finishCall(ExprPtr callee) {
    std::vector<ExprPtr> args;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (args.size() >= 8) {
                error(peek(), "Cannot have more than 8 arguments.");
            }
            args.emplace_back(expression());
        } while (match({TokenType::COMMA}));
    }

    TokPtr paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

    return std::make_shared<CallExpr>(callee, paren, args);
}

ExprPtr Parser::primary() {
    if (match(
                {TokenType::FALSE, TokenType::TRUE, 
                TokenType::NIL,
                TokenType::NUMBER, TokenType::STRING, 
                TokenType::INT, TokenType::DOUBLE})) {
        ObjPtr objP = std::make_shared<LukObject>( previous() );
        logMsg("\nIn primary Parser, before literalExpr: ", objP);
        return std::make_shared<LiteralExpr>( objP );
        // return std::make_shared<LiteralExpr>( LukObject(previous()) );
    }
    
    if (match({TokenType::SUPER})) {
      TokPtr keyword = previous();
      consume(TokenType::DOT, "Expect '.' after 'super'.");

      TokPtr method = consume(TokenType::IDENTIFIER,
          "Expect superclass method name.");

      return std::make_shared<SuperExpr>(keyword, method);
    }
    
    if (match({TokenType::THIS})) {
      auto keyword = previous();
      return std::make_shared<ThisExpr>(keyword);
    }

    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<VariableExpr>(previous());
    }
    
    // lambda function
    if (match({TokenType::FUN})) {
        return functionBody("function");
    }

    if (match({TokenType::LEFT_PAREN})) {
        ExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Exppect ')' after expression.");
        return std::make_shared<GroupingExpr>(expr);
    }
    
    
    throw error(peek(), "Expect expression.");
    return nullptr;
}

TokPtr& Parser::consume(TokenType type, std::string message) {
    if (check(type))
        return advance();
    throw error(peek(), message);
    
}

ParseError Parser::error(TokPtr& tokP, const std::string& message) {
    if (tokP->type == TokenType::END_OF_FILE) {
        lukErr.error(errTitle, tokP->line, tokP->col, " at end, " + message);
    } else {
        lukErr.error(errTitle, tokP->line, tokP->col, 
                "at '" + tokP->lexeme + "' " + message);
    }

    throw ParseError(message, tokP);
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

TokPtr& Parser::previous() {
    return m_tokens[m_current - 1];
}

TokPtr& Parser::advance() {
    if (!isAtEnd())
        ++m_current;
    return previous();
}

TokPtr& Parser::peek() {
    return m_tokens[m_current];
}

bool Parser::isAtEnd() {
    return peek()->type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek()->type == type;
}

bool Parser::checkNext(TokenType type) {
    if (isAtEnd()) return false;
    if (m_tokens[m_current+1]->type == TokenType::END_OF_FILE) return false;
    return m_tokens[m_current+1]->type == type;
}


void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous()->type == TokenType::SEMICOLON) return;

        switch(peek()->type) {
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
